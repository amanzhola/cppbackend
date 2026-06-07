# 🐘 PostgreSQL — Lesson 7/8

## 📌 Тема урока

| 📌 Тема            | 📖 Что изучается                                            | 🧠 Практический результат                                                 |
| ------------------ | ----------------------------------------------------------- | ------------------------------------------------------------------------- |
| Паттерн Repository | Сравнение наивного подхода с архитектурой через репозиторий | Бизнес-логика отделена от PostgreSQL, SQL-запросов и библиотеки `libpqxx` |

---

## 🗂 Структура практик

| 📌 Практика             | 📖 Что изучается                                                                          | ✅ Результат                                                |
| ----------------------- | ----------------------------------------------------------------------------------------- | ---------------------------------------------------------- |
| `01_naive_approach`     | Наивный подход: доменный класс сам содержит SQL и методы `LoadFromDB` / `SaveToDB`        | Видно нарушение SRP и смешивание бизнес-логики с хранением |
| `02_repository_pattern` | Паттерн Repository: `Movie`, `MovieRepository`, `PostgresMovieRepository`, `MovieUseCase` | Доменная модель отделена от PostgreSQL                     |

---

## 🧠 Главная идея урока

| ❌ Наивный подход                  | ✅ Repository                          |
| --------------------------------- | ------------------------------------- |
| `Movie` знает SQL                 | `Movie` не знает SQL                  |
| `Movie` зависит от `pqxx`         | `Movie` не зависит от БД              |
| Бизнес-логика смешана с хранением | Хранение вынесено в репозиторий       |
| Трудно заменить PostgreSQL        | Можно заменить реализацию репозитория |
| Нарушается SRP                    | Ответственности разделены             |

Главная мысль:

```text
Предметный объект не должен знать, где он хранится.
```

---

## 📚 Термины

| Термин       | Расшифровка                     | Значение                                |
| ------------ | ------------------------------- | --------------------------------------- |
| `Repository` | хранилище                       | Слой для сохранения и загрузки объектов |
| `SRP`        | Single Responsibility Principle | Принцип единственной ответственности    |
| `UUID`       | Universally Unique Identifier   | Универсальный уникальный идентификатор  |
| `UseCase`    | сценарий использования          | Код бизнес-действия приложения          |
| `libpqxx`    | PostgreSQL C++ client           | C++ библиотека для работы с PostgreSQL  |

---

## ❌ Практика 1 — `01_naive_approach`

| 📌 Элемент         | 📖 Что происходит                 |
| ------------------ | --------------------------------- |
| `Movie`            | Хранит данные фильма              |
| `LoadFromDB`       | Загружает фильм из PostgreSQL     |
| `SaveToDB`         | Сохраняет фильм в PostgreSQL      |
| `pqxx::connection` | Передаётся прямо в доменный класс |
| SQL                | Находится внутри `Movie`          |

Наивный пример:

```cpp
class Movie {
public:
    static std::optional<Movie> LoadFromDB(pqxx::connection& conn, Id id);

    void SaveToDB(pqxx::connection& conn) const;

private:
    Id id_;
    std::string title_;
    int year_;
};
```

---

## ⚠️ Почему это плохо

| Ответственность   | Почему не должна быть в `Movie`               |
| ----------------- | --------------------------------------------- |
| Данные фильма     | Это нормальная ответственность `Movie`        |
| SQL-запросы       | Это ответственность слоя хранения             |
| PostgreSQL        | Доменная модель не должна знать конкретную БД |
| `pqxx::work`      | Транзакции относятся к инфраструктуре         |
| Mapping SQL → C++ | Это задача репозитория                        |

Проблема:

```text
Movie теперь отвечает сразу за несколько вещей.
```

Это нарушает:

```text
Single Responsibility Principle
```

---

## 🧱 Что будет при изменениях

| Изменение                     | Что придётся менять в наивном подходе |
| ----------------------------- | ------------------------------------- |
| PostgreSQL заменить на SQLite | `Movie`                               |
| Изменить структуру таблицы    | `Movie`                               |
| Заменить `libpqxx`            | `Movie`                               |
| Перейти на REST API           | `Movie`                               |
| Изменить бизнес-логику фильма | `Movie`                               |

Итог:

```text
у класса слишком много причин для изменения
```

---

## ✅ Практика 2 — `02_repository_pattern`

| Слой                      | Назначение                            |
| ------------------------- | ------------------------------------- |
| `Movie`                   | Доменная модель, хранит только данные |
| `MovieRepository`         | Интерфейс хранения                    |
| `PostgresMovieRepository` | Реализация хранения через PostgreSQL  |
| `MovieUseCase`            | Бизнес-сценарий                       |
| `PostgreSQL`              | Физическое хранилище                  |

Схема:

```text
MovieUseCase
    ↓
MovieRepository interface
    ↑
PostgresMovieRepository
    ↓
PostgreSQL
```

---

## 🎬 Доменная модель Movie

Правильный `Movie`:

```cpp
class Movie {
public:
    Id GetId() const;
    std::string_view GetTitle() const;
    int GetYear() const;

private:
    Id id_;
    std::string title_;
    int year_;
};
```

`Movie` не знает про:

```text
SQL
PostgreSQL
pqxx
таблицы
транзакции
```

---

## 🧩 Интерфейс репозитория

```cpp
class MovieRepository {
public:
    virtual ~MovieRepository() = default;

    virtual void Save(const Movie& movie) = 0;

    virtual std::optional<Movie> Load(const Movie::Id& id) = 0;

    virtual std::vector<Movie> LoadAll() = 0;
};
```

| Метод     | Назначение              |
| --------- | ----------------------- |
| `Save`    | Сохранить фильм         |
| `Load`    | Загрузить один фильм    |
| `LoadAll` | Загрузить все фильмы    |
| `= 0`     | Чисто виртуальный метод |

---

## 🐘 PostgreSQL-репозиторий

```cpp
class PostgresMovieRepository : public MovieRepository {
};
```

Именно здесь находятся:

```text
pqxx::connection
pqxx::work
pqxx::read_transaction
SQL
prepared queries
```

---

## 🆔 UUID

В уроке используется:

```cpp
boost::uuids::uuid
```

Расшифровка:

```text
UUID = Universally Unique Identifier
```

Пример:

```text
0a785520-9827-4b80-aed6-f15b9a0f4249
```

Преимущества:

| Причина                     | Значение                                  |
| --------------------------- | ----------------------------------------- |
| Глобальная уникальность     | Можно создавать ID без обращения к БД     |
| Независимость от PostgreSQL | ID не зависит от `SERIAL`                 |
| Распределённые системы      | Разные серверы могут создавать ID         |
| Удобство переноса           | Объект можно сохранить в другое хранилище |

---

## 🛠 Подключения Boost UUID

```cpp
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
```

Важно:

```cpp
#include <boost/uuid/string_generator.hpp>
```

нужен для:

```cpp
boost::uuids::string_generator
```

---

## 💾 Таблица movies

```sql
CREATE TABLE IF NOT EXISTS movies (
    id uuid PRIMARY KEY,
    title varchar(200) NOT NULL,
    year integer NOT NULL
);
```

| Поле          | Назначение               |
| ------------- | ------------------------ |
| `id uuid`     | UUID фильма              |
| `title`       | Название                 |
| `year`        | Год                      |
| `PRIMARY KEY` | Уникальный идентификатор |

---

## ♻️ Save с ON CONFLICT

```sql
INSERT INTO movies (id, title, year) VALUES ($1, $2, $3)
ON CONFLICT (id) DO UPDATE
SET title = EXCLUDED.title,
    year = EXCLUDED.year
```

| SQL                | Значение                     |
| ------------------ | ---------------------------- |
| `ON CONFLICT (id)` | Если такой ID уже есть       |
| `DO UPDATE`        | Обновить существующую строку |
| `EXCLUDED`         | Новые значения из INSERT     |

---

## 🧪 Сборка

```bash
cd ~/cppbackend/lessons/sprint_20_20/postgres/lesson_7_8/02_repository_pattern

mkdir build
cd build

conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

---

## ▶️ Запуск

```bash
./bin/repository_example "postgres://postgres:postgres@localhost:30432/postgres"
```

---

## 🧯 Возникшие ошибки и исправления

| Ошибка                                             | Причина                                                         | Исправление                                           |
| -------------------------------------------------- | --------------------------------------------------------------- | ----------------------------------------------------- |
| `string_generator is not a member of boost::uuids` | Не подключён заголовок                                          | Добавить `#include <boost/uuid/string_generator.hpp>` |
| `Connection refused`                               | Docker-контейнер PostgreSQL остановлен                          | `sudo docker start postgres16`                        |
| `invalid input syntax for type integer`            | Старая таблица `movies` имела `id SERIAL`, а новая ждала `uuid` | `DROP TABLE movies;`                                  |

---

## 🧹 Исправление старой таблицы

Если раньше создавалась таблица `movies` с `SERIAL`, удалить её:

```bash
psql -h localhost -p 30432 -U postgres -W
```

```sql
DROP TABLE movies;
```

После этого программа создаст таблицу заново:

```sql
id uuid PRIMARY KEY
```

---

## ✅ Результат

Пример вывода:

```text
0a785520-9827-4b80-aed6-f15b9a0f4249 | The King's Speech | 2010
b1b99a05-50a7-4b2c-a4b9-8abab321f7bb | The Terminal | 2004
6778d5a8-fc72-4049-abb3-7cd17fd1eaf8 | Groundhog Day | 1993
```

---

## 🏁 Итог

| 📌 Что сделано                       | 📖 Почему это важно                     |
| ------------------------------------ | --------------------------------------- |
| Рассмотрен наивный подход            | Видно нарушение SRP                     |
| Выделен `MovieRepository`            | Бизнес-код зависит от интерфейса        |
| Реализован `PostgresMovieRepository` | SQL изолирован в одном слое             |
| Использован UUID                     | Идентификаторы не зависят от БД         |
| Использован `ON CONFLICT`            | Повторное сохранение обновляет строку   |
| Исправлены ошибки запуска            | Практика доведена до рабочего состояния |

Главная мысль:

```text
Repository отделяет бизнес-логику от хранения данных.
```

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
