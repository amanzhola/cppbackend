# 🏁 Leave Game

## 📌 Описание задачи

| 📌 Блок                | 📖 Что реализуется                                                                         | 🧠 Практическая ценность                                 | ✅ Результат                   |
| ---------------------- | ------------------------------------------------------------------------------------------ | -------------------------------------------------------- | ----------------------------- |
| PostgreSQL в игре      | `GAME_DB_URL`, `libpqxx`, `ConnectionPool`, таблица `retired_players`                      | Сервер сохраняет результаты игроков после выхода из игры | Рекорды хранятся в БД         |
| Завершение игры        | `dogRetirementTime`, idle-time, play-time, удаление собаки и игрока                        | Неактивные игроки автоматически завершают игру           | Игроки попадают в leaderboard |
| Таблица лидеров        | `/api/v1/game/records`, `start`, `maxItems`, сортировка по `score`, `play_time_ms`, `name` | Клиент может получать рекорды через HTTP API             | Добавлен endpoint records     |
| Безопасная работа с БД | Пул соединений, RAII-обёртка, транзакции `pqxx::work` и `pqxx::read_transaction`           | Сервер может работать с БД из нескольких потоков         | Соединения переиспользуются   |

---

## 📂 Путь решения

| 📌 Каталог                                          | 📖 Назначение                                        |
| --------------------------------------------------- | ---------------------------------------------------- |
| `~/cppbackend/sprint4/problems/leave_game/solution` | Итоговое решение задачи                              |
| `src/db.h`                                          | Пул соединений и репозиторий рекордов                |
| `src/model.h`, `src/model.cpp`                      | Учёт времени игры и idle-time собаки                 |
| `src/request_handler.h`, `src/request_handler.cpp`  | Endpoint `/api/v1/game/records`                      |
| `src/main.cpp`                                      | Инициализация БД, retirement-логика и records-loader |

---

## 🧩 Основные изменения

| Файл                        | Что добавить / изменить                                               | Зачем                               |
| --------------------------- | --------------------------------------------------------------------- | ----------------------------------- |
| `conanfile.txt`             | `libpqxx/7.7.4`, `boost/1.78.0`, `catch2/3.3.2`                       | Зависимости для PostgreSQL и тестов |
| `CMakeLists.txt`            | Добавить `src/db.h` в список файлов                                   | Чтобы файл был частью проекта       |
| `src/db.h`                  | `ConnectionPool`, `Record`, `RetiredPlayersRepository`                | Работа с БД и таблицей рекордов     |
| `src/model.h`               | `AddGameTime`, `GetPlayTime`, `GetIdleTime`, `RemoveDog`              | Учёт активности собаки              |
| `src/model.cpp`             | Реализация времени игры и удаления собаки                             | Retirement-логика                   |
| `src/players.h`             | `Remove(Player::Id id)`                                               | Удаление завершившего игрока        |
| `src/player_tokens.h`       | `RemovePlayer(Player::Id player_id)`                                  | Аннулирование токена                |
| `src/json_loader.h/cpp`     | `dog_retirement_time` из `dogRetirementTime`                          | Настройка времени до выхода из игры |
| `src/request_handler.h/cpp` | `kRecordsEndpoint`, `HandleRecords`, `records_loader_`                | API рекордов                        |
| `src/main.cpp`              | `GAME_DB_URL`, пул соединений, репозиторий, `retire_inactive_players` | Интеграция БД в сервер              |

---

## 📦 conanfile.txt

```txt
[requires]
boost/1.78.0
catch2/3.3.2
libpqxx/7.7.4

[generators]
cmake
```

---

## 🏗 CMakeLists.txt

| Что изменить              | Что добавить |
| ------------------------- | ------------ |
| В список `add_executable` | `src/db.h`   |

```cmake
    src/db.h
```

---

## 🗄 src/db.h

| Компонент                                | Назначение                | Важные детали                                        |
| ---------------------------------------- | ------------------------- | ---------------------------------------------------- |
| `ConnectionPool`                         | Пул соединений PostgreSQL | Выдаёт и возвращает соединения                       |
| `ConnectionWrapper`                      | RAII-обёртка              | В деструкторе возвращает соединение в пул            |
| `Record`                                 | DTO записи leaderboard    | `name`, `score`, `play_time_ms`                      |
| `RetiredPlayersRepository::Init()`       | Создаёт таблицу и индекс  | `retired_players`, `retired_players_leaderboard_idx` |
| `RetiredPlayersRepository::Save()`       | Сохраняет рекорд          | `INSERT INTO retired_players`                        |
| `RetiredPlayersRepository::GetRecords()` | Читает leaderboard        | `ORDER BY score DESC, play_time_ms ASC, name ASC`    |

```cpp
#pragma once

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <string>
#include <utility>
#include <vector>

namespace db {

class ConnectionPool {
    using ConnectionPtr = std::shared_ptr<pqxx::connection>;

public:
    class ConnectionWrapper {
    public:
        ConnectionWrapper(ConnectionPtr&& conn, ConnectionPool& pool) noexcept
            : conn_{std::move(conn)}
            , pool_{&pool} {
        }

        ConnectionWrapper(const ConnectionWrapper&) = delete;
        ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;

        ConnectionWrapper(ConnectionWrapper&&) = default;
        ConnectionWrapper& operator=(ConnectionWrapper&&) = default;

        pqxx::connection& operator*() const noexcept {
            return *conn_;
        }

        pqxx::connection* operator->() const noexcept {
            return conn_.get();
        }

        ~ConnectionWrapper() {
            if (conn_) {
                pool_->ReturnConnection(std::move(conn_));
            }
        }

    private:
        ConnectionPtr conn_;
        ConnectionPool* pool_;
    };

    template <typename Factory>
    ConnectionPool(size_t capacity, Factory&& factory) {
        pool_.reserve(capacity);

        for (size_t i = 0; i < capacity; ++i) {
            pool_.push_back(factory());
        }
    }

    ConnectionWrapper GetConnection() {
        std::unique_lock lock{mutex_};

        cond_var_.wait(lock, [this] {
            return used_connections_ < pool_.size();
        });

        return {std::move(pool_[used_connections_++]), *this};
    }

private:
    void ReturnConnection(ConnectionPtr&& conn) {
        {
            std::lock_guard lock{mutex_};

            assert(used_connections_ > 0);

            pool_[--used_connections_] = std::move(conn);
        }

        cond_var_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::vector<ConnectionPtr> pool_;
    size_t used_connections_ = 0;
};

struct Record {
    std::string name;
    int score = 0;
    std::int64_t play_time_ms = 0;
};

class RetiredPlayersRepository {
public:
    explicit RetiredPlayersRepository(ConnectionPool& pool)
        : pool_{pool} {
    }

    void Init() {
        auto conn = pool_.GetConnection();

        pqxx::work tx{*conn};

        tx.exec(R"(
            CREATE EXTENSION IF NOT EXISTS pgcrypto;

            CREATE TABLE IF NOT EXISTS retired_players (
                id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
                name text NOT NULL,
                score integer NOT NULL CHECK (score >= 0),
                play_time_ms bigint NOT NULL CHECK (play_time_ms >= 0)
            );

            CREATE INDEX IF NOT EXISTS retired_players_leaderboard_idx
            ON retired_players (
                score DESC,
                play_time_ms ASC,
                name ASC
            );
        )");

        tx.commit();
    }

    void Save(std::string name, int score, std::int64_t play_time_ms) {
        auto conn = pool_.GetConnection();

        pqxx::work tx{*conn};

        tx.exec_params(
            "INSERT INTO retired_players (name, score, play_time_ms) "
            "VALUES ($1, $2, $3)",
            name,
            score,
            play_time_ms
        );

        tx.commit();
    }

    std::vector<Record> GetRecords(size_t start, size_t max_items) {
        auto conn = pool_.GetConnection();

        pqxx::read_transaction tx{*conn};

        std::vector<Record> result;

        const std::string query =
            "SELECT name, score, play_time_ms "
            "FROM retired_players "
            "ORDER BY score DESC, play_time_ms ASC, name ASC "
            "LIMIT " + std::to_string(max_items) +
            " OFFSET " + std::to_string(start);

        for (auto [name, score, play_time_ms] :
             tx.query<std::string, int, std::int64_t>(query)) {
            result.push_back({std::move(name), score, play_time_ms});
        }

        return result;
    }

private:
    ConnectionPool& pool_;
};

}  // namespace db
```

---

## 🐕 model.h и model.cpp

| Класс         | Что добавить                     | Зачем                                         |
| ------------- | -------------------------------- | --------------------------------------------- |
| `Dog`         | `play_time_`, `idle_time_`       | Хранить общее время игры и время без движения |
| `Dog`         | `AddGameTime(delta)`             | Увеличивать play-time и idle-time             |
| `Dog`         | `GetPlayTime()`, `GetIdleTime()` | Проверять retirement и сохранять рекорд       |
| `GameSession` | `RemoveDog(Dog::Id id)`          | Удалять собаку после завершения игры          |

В `Dog`:

```cpp
void AddGameTime(std::chrono::milliseconds delta) noexcept;

std::chrono::milliseconds GetPlayTime() const noexcept {
    return play_time_;
}

std::chrono::milliseconds GetIdleTime() const noexcept {
    return idle_time_;
}
```

Поля:

```cpp
std::chrono::milliseconds play_time_{0};
std::chrono::milliseconds idle_time_{0};
```

Реализация:

```cpp
void Dog::AddGameTime(std::chrono::milliseconds delta) noexcept {
    play_time_ += delta;

    const Speed speed = GetSpeed();

    if (speed.dx == 0.0 && speed.dy == 0.0) {
        idle_time_ += delta;
    } else {
        idle_time_ = std::chrono::milliseconds{0};
    }
}
```

В `GameSession::Update` после движения собаки:

```cpp
dog.AddGameTime(
    std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(delta_seconds)
    )
);
```

Удаление собаки:

```cpp
void GameSession::RemoveDog(Dog::Id id) {
    dogs_.erase(id);
}
```

---

## 👤 players.h и player_tokens.h

| Файл              | Метод                                | Зачем                              |
| ----------------- | ------------------------------------ | ---------------------------------- |
| `players.h`       | `Remove(Player::Id id)`              | Удалить игрока из активных игроков |
| `player_tokens.h` | `RemovePlayer(Player::Id player_id)` | Удалить токен завершившего игрока  |

`players.h`:

```cpp
void Remove(Player::Id id) {
    players_.erase(id);
}
```

`player_tokens.h`:

```cpp
void RemovePlayer(Player::Id player_id) {
    for (auto it = token_to_player_.begin(); it != token_to_player_.end(); ) {
        if (it->second->GetId() == player_id) {
            it = token_to_player_.erase(it);
        } else {
            ++it;
        }
    }
}
```

---

## ⚙️ json_loader.h и json_loader.cpp

| Что добавить               | Где              | Значение по умолчанию            |
| -------------------------- | ---------------- | -------------------------------- |
| `dog_retirement_time`      | `GameLoadResult` | `60000 ms`                       |
| чтение `dogRetirementTime` | `LoadGame`       | секунды из config → milliseconds |

В `json_loader.h`:

```cpp
#include <chrono>
```

```cpp
std::chrono::milliseconds dog_retirement_time{60000};
```

В `json_loader.cpp` перед `return`:

```cpp
std::chrono::milliseconds dog_retirement_time{60000};

if (root_obj.contains("dogRetirementTime")) {
    const double seconds = ReadDouble(root_obj, "dogRetirementTime");

    dog_retirement_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(seconds)
    );
}
```

Return:

```cpp
return GameLoadResult{
    .game = std::move(game),
    .extra_data = std::move(extra),
    .dog_retirement_time = dog_retirement_time
};
```

---

## 🌐 request_handler.h и request_handler.cpp

| Что добавить       | Назначение                  |
| ------------------ | --------------------------- |
| `kRecordsEndpoint` | Путь `/api/v1/game/records` |
| `records_loader_`  | Функция загрузки рекордов   |
| `HandleRecords`    | Обработчик records endpoint |
| `GET` / `HEAD`     | Разрешённые методы          |
| `start`            | Offset leaderboard          |
| `maxItems`         | Limit leaderboard           |
| `maxItems > 100`   | Ошибка `400 Bad Request`    |

Endpoint:

```cpp
static constexpr std::string_view kRecordsEndpoint = "/api/v1/game/records"sv;
```

Поле:

```cpp
std::function<json::array(size_t, size_t)> records_loader_;
```

Метод:

```cpp
StringResponse HandleRecords(const StringRequest& req, std::string_view target);
```

Маршрутизация:

```cpp
if (target.starts_with(kRecordsEndpoint)) {
    return send(HandleRecords(req, target));
}
```

---

## 📡 HandleRecords

| Шаг                 | Что делает                                |
| ------------------- | ----------------------------------------- |
| Проверяет метод     | Разрешает только `GET` и `HEAD`           |
| Парсит query string | Читает `start` и `maxItems`               |
| Проверяет лимит     | `maxItems` не больше `100`                |
| Загружает записи    | Через `records_loader_(start, max_items)` |
| Возвращает JSON     | Массив рекордов                           |
| Для `HEAD`          | Очищает тело и ставит `content_length(0)` |

```cpp
RequestHandler::StringResponse RequestHandler::HandleRecords(
    const StringRequest& req,
    std::string_view target) {
    const auto version = req.version();
    const bool keep_alive = req.keep_alive();

    if (req.method() != http::verb::get && req.method() != http::verb::head) {
        auto response = MakeErrorResponse(
            version,
            keep_alive,
            http::status::method_not_allowed,
            "invalidMethod",
            "Invalid method"
        );

        response.set(http::field::allow, "GET, HEAD");

        return response;
    }

    size_t start = 0;
    size_t max_items = 100;

    const auto question_pos = target.find('?');

    if (question_pos != std::string_view::npos) {
        const std::string query{target.substr(question_pos + 1)};

        size_t pos = 0;

        while (pos < query.size()) {
            const size_t amp = query.find('&', pos);

            const std::string part = query.substr(
                pos,
                amp == std::string::npos ? std::string::npos : amp - pos
            );

            const size_t eq = part.find('=');

            if (eq != std::string::npos) {
                const std::string key = part.substr(0, eq);
                const std::string value = part.substr(eq + 1);

                if (key == "start") {
                    start = static_cast<size_t>(std::stoull(value));
                } else if (key == "maxItems") {
                    max_items = static_cast<size_t>(std::stoull(value));
                }
            }

            if (amp == std::string::npos) {
                break;
            }

            pos = amp + 1;
        }
    }

    if (max_items > 100) {
        return MakeBadRequest(
            version,
            keep_alive,
            "invalidArgument",
            "maxItems must be not greater than 100"
        );
    }

    json::array records = records_loader_(start, max_items);

    auto response = MakeJsonResponse(version, keep_alive, std::move(records));

    if (req.method() == http::verb::head) {
        response.body().clear();
        response.content_length(0);
    }

    return response;
}
```

---

## 🚀 main.cpp

| Что добавить                | Зачем                               |
| --------------------------- | ----------------------------------- |
| `#include "db.h"`           | Доступ к пулу и репозиторию         |
| `GAME_DB_URL`               | URL подключения к PostgreSQL        |
| `ConnectionPool`            | Пул соединений по числу потоков     |
| `RetiredPlayersRepository`  | Работа с таблицей `retired_players` |
| `records_repository.Init()` | Создание таблицы и индекса          |
| `retire_inactive_players`   | Завершение idle-игроков             |
| `records_loader`            | JSON для `/api/v1/game/records`     |

Include:

```cpp
#include "db.h"
```

После загрузки config:

```cpp
const auto dog_retirement_time = load_result.dog_retirement_time;
```

Инициализация БД:

```cpp
const char* game_db_url = std::getenv("GAME_DB_URL");

if (!game_db_url) {
    throw std::runtime_error("GAME_DB_URL is not specified");
}

db::ConnectionPool connection_pool{
    num_threads,
    [game_db_url] {
        return std::make_shared<pqxx::connection>(game_db_url);
    }
};

db::RetiredPlayersRepository records_repository{connection_pool};

records_repository.Init();
```

---

## 💤 retire_inactive_players

| Шаг                          | Что делает                                  |
| ---------------------------- | ------------------------------------------- |
| Проходит по активным игрокам | `players.GetAll()`                          |
| Находит собаку игрока        | `session.FindDog(player.GetDogId())`        |
| Проверяет idle-time          | `dog->GetIdleTime() >= dog_retirement_time` |
| Сохраняет рекорд             | `records_repository.Save(...)`              |
| Удаляет собаку               | `session.RemoveDog(dog->GetId())`           |
| Удаляет токен                | `player_tokens.RemovePlayer(player_id)`     |
| Удаляет игрока               | `players.Remove(player_id)`                 |

```cpp
auto retire_inactive_players = [&players,
                                &player_tokens,
                                &records_repository,
                                dog_retirement_time] {
    std::vector<app::Player::Id> players_to_remove;

    for (const auto& [player_id, player] : players.GetAll()) {
        model::GameSession& session = player.GetSession();

        model::Dog* dog = session.FindDog(player.GetDogId());

        if (!dog) {
            players_to_remove.push_back(player_id);
            continue;
        }

        if (dog->GetIdleTime() >= dog_retirement_time) {
            records_repository.Save(
                dog->GetName(),
                dog->GetScore(),
                dog->GetPlayTime().count()
            );

            session.RemoveDog(dog->GetId());

            players_to_remove.push_back(player_id);
        }
    }

    for (app::Player::Id player_id : players_to_remove) {
        player_tokens.RemovePlayer(player_id);
        players.Remove(player_id);
    }
};
```

---

## 📜 records_loader

| Поле JSON  | Откуда берётся                 | Формат            |
| ---------- | ------------------------------ | ----------------- |
| `name`     | `record.name`                  | string            |
| `score`    | `record.score`                 | integer           |
| `playTime` | `record.play_time_ms / 1000.0` | seconds as double |

```cpp
[&records_repository](size_t start, size_t max_items) {
    json::array result;

    for (const db::Record& record : records_repository.GetRecords(start, max_items)) {
        json::object obj;

        obj["name"] = record.name;
        obj["score"] = record.score;
        obj["playTime"] = static_cast<double>(record.play_time_ms) / 1000.0;

        result.emplace_back(std::move(obj));
    }

    return result;
}
```

---

## 🔁 Tick callback

| Где                | Что делать                                                             |
| ------------------ | ---------------------------------------------------------------------- |
| `on_tick` callback | Вызвать `retire_inactive_players()` перед `state_saver->OnTick(delta)` |
| ticker lambda      | После `game.Update(delta_seconds)` вызвать `retire_inactive_players()` |

```cpp
[&state_saver, &retire_inactive_players](std::chrono::milliseconds delta) {
    retire_inactive_players();

    if (state_saver) {
        state_saver->OnTick(delta);
    }
}
```

После обновления игры:

```cpp
retire_inactive_players();
```

---

## 🧪 Сборка

```bash
cd ~/cppbackend/sprint4/problems/leave_game/solution

rm -rf build

mkdir build

cd build

conan install .. \
  --build=missing \
  -s build_type=Release \
  -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск

```bash
export GAME_DB_URL="postgres://postgres:postgres@localhost:30432/postgres"

./bin/game_server \
  --config-file ../data/config.json \
  --www-root ../static
```

---

## 🌐 Проверка API

| Проверка             | Команда                                                                | Ожидание           |
| -------------------- | ---------------------------------------------------------------------- | ------------------ |
| Получить records     | `curl "http://localhost:8080/api/v1/game/records"`                     | JSON-массив        |
| Проверить pagination | `curl "http://localhost:8080/api/v1/game/records?start=0&maxItems=10"` | До 10 записей      |
| Проверить лимит      | `curl -i "http://localhost:8080/api/v1/game/records?maxItems=101"`     | `400 Bad Request`  |
| Проверить метод HEAD | `curl -I "http://localhost:8080/api/v1/game/records"`                  | Заголовки без тела |

```bash
curl "http://localhost:8080/api/v1/game/records"

curl "http://localhost:8080/api/v1/game/records?start=0&maxItems=10"

curl -i "http://localhost:8080/api/v1/game/records?maxItems=101"

curl -I "http://localhost:8080/api/v1/game/records"
```

---

## ✅ Что закрывает патч

| Требование                            | Статус |
| ------------------------------------- | ------ |
| `GAME_DB_URL`                         | ✅      |
| PostgreSQL через `libpqxx`            | ✅      |
| `ConnectionPool`                      | ✅      |
| Таблица `retired_players`             | ✅      |
| Индекс leaderboard                    | ✅      |
| `dogRetirementTime`                   | ✅      |
| Учёт `play_time`                      | ✅      |
| Учёт `idle_time`                      | ✅      |
| Автоматическое завершение idle-собаки | ✅      |
| Сохранение рекорда                    | ✅      |
| Удаление собаки                       | ✅      |
| Удаление игрока                       | ✅      |
| Аннулирование токена                  | ✅      |
| `GET /api/v1/game/records`            | ✅      |
| `HEAD /api/v1/game/records`           | ✅      |
| `start` / `maxItems`                  | ✅      |
| `maxItems > 100 -> 400`               | ✅      |
| Сортировка leaderboard                | ✅      |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add sprint4/problems/leave_game/solution

git commit -m "Implement leave game records storage"

git push
```

---

## ⬅️ Назад

[Вернуться к Diploma](../../../../lessons/sprint_20_20/diploma/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
