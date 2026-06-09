# 🎮 Lesson 2/7 — Проектируем БД для игры

## 📌 Описание урока

| 📌 Блок            | 📖 Что изучается                                            | 🧠 Практическая ценность                          | ✅ Результат                             |
| ------------------ | ----------------------------------------------------------- | ------------------------------------------------- | --------------------------------------- |
| Таблица игроков    | `retired_players`, завершившие игру игроки                  | Позволяет сохранять результат после выхода игрока | Игроки сохраняются в PostgreSQL         |
| Поля статистики    | `name`, `score`, `play_time_ms`                             | Позволяет хранить имя, очки и время игры          | Данные пригодны для leaderboard         |
| UUID               | `uuid PRIMARY KEY DEFAULT gen_random_uuid()`                | Позволяет создавать независимые идентификаторы    | Освоено использование UUID              |
| Ограничения        | `NOT NULL`, `CHECK(score >= 0)`, `CHECK(play_time_ms >= 0)` | Защищают таблицу от некорректных данных           | Отрицательные значения запрещены        |
| Индекс leaderboard | `score DESC`, `play_time_ms ASC`, `name ASC`                | Ускоряет сортировку таблицы лидеров               | Подготовлен индекс под финальный запрос |

---

## 📂 Структура урока

| 📄 Файл                                  | 📖 Назначение                                            | ▶️ Запуск                                                                                      |
| ---------------------------------------- | -------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| `README.md`                              | Конспект урока и описание практик                        | Читать как инструкцию                                                                          |
| `practice_01_create_retired_players.sql` | Создание таблицы `retired_players` и индекса leaderboard | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_create_retired_players.sql` |
| `practice_02_insert_retired_players.sql` | Вставка тестовых игроков                                 | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_insert_retired_players.sql` |
| `practice_03_select_leaderboard.sql`     | Получение таблицы лидеров                                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_select_leaderboard.sql`     |
| `practice_04_explain_index.sql`          | Проверка плана сортировки через `EXPLAIN`                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_04_explain_index.sql`          |

---

## 🧠 Основные понятия

| Термин    | Расшифровка                   | Значение                                            | Пример                                 |
| --------- | ----------------------------- | --------------------------------------------------- | -------------------------------------- |
| БД        | База данных                   | Хранилище структурированных данных                  | PostgreSQL                             |
| `retired` | завершивший участие           | Игрок больше не находится в активной игровой сессии | `retired_players`                      |
| `players` | игроки                        | Пользователи игрового сервера                       | `name`, `score`                        |
| `ms`      | millisecond                   | Одна тысячная секунды                               | `1000 ms = 1 секунда`                  |
| `UUID`    | Universally Unique Identifier | Универсально уникальный идентификатор               | `550e8400-e29b-41d4-a716-446655440000` |
| `DESC`    | descending                    | Сортировка по убыванию                              | Больший `score` выше                   |
| `ASC`     | ascending                     | Сортировка по возрастанию                           | Меньшее время выше                     |

---

## 🧱 Что нужно хранить после выхода игрока

| Поле           | Тип       | Что хранит                      | Почему такой тип                 |
| -------------- | --------- | ------------------------------- | -------------------------------- |
| `id`           | `uuid`    | Уникальный идентификатор записи | Можно генерировать независимо    |
| `name`         | `text`    | Имя игрока                      | Имя может быть разной длины      |
| `score`        | `integer` | Баллы игрока                    | Очки удобно хранить целым числом |
| `play_time_ms` | `bigint`  | Время игры в миллисекундах      | Миллисекунд может быть много     |

---

## 🧪 Практика 1 — создать таблицу retired_players

| SQL-часть                                                | Что делает                     | Зачем нужно                          |
| -------------------------------------------------------- | ------------------------------ | ------------------------------------ |
| `CREATE EXTENSION IF NOT EXISTS pgcrypto;`               | Подключает `pgcrypto`          | Нужна функция `gen_random_uuid()`    |
| `DROP TABLE IF EXISTS retired_players;`                  | Удаляет старую таблицу         | Практику можно перезапускать         |
| `id uuid PRIMARY KEY DEFAULT gen_random_uuid()`          | Создаёт UUID-ключ              | ID создаётся автоматически           |
| `name text NOT NULL`                                     | Имя игрока обязательно         | Нельзя сохранить игрока без имени    |
| `score integer NOT NULL CHECK (score >= 0)`              | Баллы неотрицательные          | Нельзя сохранить отрицательный счёт  |
| `play_time_ms bigint NOT NULL CHECK (play_time_ms >= 0)` | Время неотрицательное          | Нельзя сохранить отрицательное время |
| `CREATE INDEX retired_players_leaderboard_idx ...`       | Создаёт индекс для leaderboard | Ускоряет сортировку лидеров          |

Файл `practice_01_create_retired_players.sql`:

```sql
CREATE EXTENSION IF NOT EXISTS pgcrypto;

DROP TABLE IF EXISTS retired_players;

CREATE TABLE retired_players (
    id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    name text NOT NULL,
    score integer NOT NULL CHECK (score >= 0),
    play_time_ms bigint NOT NULL CHECK (play_time_ms >= 0)
);

CREATE INDEX retired_players_leaderboard_idx
ON retired_players (
    score DESC,
    play_time_ms ASC,
    name ASC
);

\d retired_players
```

Запуск:

```bash
psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_create_retired_players.sql
```

---

## 🏆 Почему нужен индекс leaderboard

| Критерий       | Направление | Почему                                         |
| -------------- | ----------- | ---------------------------------------------- |
| `score`        | `DESC`      | Чем больше очков, тем выше игрок               |
| `play_time_ms` | `ASC`       | При равных очках быстрее сыгравший выше        |
| `name`         | `ASC`       | При равных очках и времени сортировка по имени |

Индекс:

```sql
CREATE INDEX retired_players_leaderboard_idx
ON retired_players (
    score DESC,
    play_time_ms ASC,
    name ASC
);
```

---

## 🧪 Практика 2 — вставить игроков

| Игрок     | Score | Play time | Назначение                       |
| --------- | ----- | --------- | -------------------------------- |
| `Bob`     | `100` | `5000`    | Низкий score                     |
| `Alice`   | `200` | `8000`    | Высокий score, но больше времени |
| `Charlie` | `200` | `6000`    | Высокий score и меньше времени   |
| `David`   | `200` | `6000`    | Проверка сортировки по имени     |
| `Eve`     | `150` | `3000`    | Средний score                    |

Файл `practice_02_insert_retired_players.sql`:

```sql
INSERT INTO retired_players (name, score, play_time_ms)
VALUES
    ('Bob', 100, 5000),
    ('Alice', 200, 8000),
    ('Charlie', 200, 6000),
    ('David', 200, 6000),
    ('Eve', 150, 3000);

SELECT *
FROM retired_players;
```

Запуск:

```bash
psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_insert_retired_players.sql
```

---

## 🧪 Практика 3 — таблица лидеров

| SQL-часть                          | Что делает                                 |
| ---------------------------------- | ------------------------------------------ |
| `SELECT name, score, play_time_ms` | Показывает нужные поля                     |
| `FROM retired_players`             | Берёт завершивших игру игроков             |
| `ORDER BY score DESC`              | Лучшие по очкам выше                       |
| `play_time_ms ASC`                 | При равных очках быстрее сыгравший выше    |
| `name ASC`                         | При равных очках и времени имя по алфавиту |

Файл `practice_03_select_leaderboard.sql`:

```sql
SELECT
    name,
    score,
    play_time_ms
FROM retired_players
ORDER BY
    score DESC,
    play_time_ms ASC,
    name ASC;
```

Запуск:

```bash
psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_select_leaderboard.sql
```

Ожидаемый порядок:

```text
Charlie 200 6000
David   200 6000
Alice   200 8000
Eve     150 3000
Bob     100 5000
```

---

## 🧪 Практика 4 — проверить индекс через EXPLAIN

| Термин            | Значение                            | Почему важно                                   |
| ----------------- | ----------------------------------- | ---------------------------------------------- |
| `EXPLAIN`         | Показать план выполнения            | Видно, как PostgreSQL собирается читать данные |
| `Seq Scan`        | Sequential Scan                     | Последовательное чтение таблицы                |
| `Index Scan`      | Чтение через индекс                 | Может использоваться на большой таблице        |
| Маленькая таблица | PostgreSQL может выбрать `Seq Scan` | Это нормально, потому что строк мало           |
| Большая таблица   | Индекс становится полезнее          | Leaderboard читается быстрее                   |

Файл `practice_04_explain_index.sql`:

```sql
EXPLAIN
SELECT
    name,
    score,
    play_time_ms
FROM retired_players
ORDER BY
    score DESC,
    play_time_ms ASC,
    name ASC
LIMIT 10;
```

Запуск:

```bash
psql -h localhost -p 30432 -U postgres -d postgres -f practice_04_explain_index.sql
```

---

## ✅ Главное запомнить

| Тема              | Правило                                        | Пример                                   |
| ----------------- | ---------------------------------------------- | ---------------------------------------- |
| `retired_players` | Хранит игроков после завершения игры           | Игрок вышел → запись в БД                |
| `UUID`            | Удобен для независимых ID                      | `gen_random_uuid()`                      |
| `CHECK`           | Защищает от плохих значений                    | `score >= 0`                             |
| `NOT NULL`        | Запрещает пустые обязательные поля             | `name text NOT NULL`                     |
| `bigint`          | Подходит для миллисекунд                       | `play_time_ms bigint`                    |
| Leaderboard       | Сортировка по трём полям                       | `score DESC, play_time_ms ASC, name ASC` |
| Индекс            | Должен совпадать с сортировкой                 | `retired_players_leaderboard_idx`        |
| `EXPLAIN`         | План может быть `Seq Scan` на маленьких данных | Это не ошибка                            |

---

## 🏁 Итог

| 📌 Что сделано                             | ✅ Статус |
| ------------------------------------------ | -------- |
| Создана папка `lesson_2_7`                 | ✅        |
| Создан `README.md`                         | ✅        |
| Создана таблица `retired_players`          | ✅        |
| Добавлен UUID primary key                  | ✅        |
| Добавлены ограничения `NOT NULL` и `CHECK` | ✅        |
| Создан мультииндекс для leaderboard        | ✅        |
| Добавлены тестовые игроки                  | ✅        |
| Проверена сортировка лидеров               | ✅        |
| Добавлен `EXPLAIN`                         | ✅        |
| Подготовлена основа для дипломной БД       | ✅        |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add lessons/sprint_20_20/diploma/lesson_2_7

git commit -m "Add diploma database design lesson practices"

git push
```

---

## ⬅️ Назад

[Вернуться к Diploma](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
