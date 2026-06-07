# 🐘 PostgreSQL — Lesson 6/8

## 📌 Тема урока

| 📌 Тема               | 📖 Что изучается                                                                                    | 🧠 Практический результат                                                           |
| --------------------- | --------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------- |
| C++ клиент PostgreSQL | `libpqxx`, соединение с БД, транзакции, чтение результатов, `NULL`, SQL injection, prepared queries | Освоено подключение C++ приложения к PostgreSQL и безопасная работа с SQL-запросами |

---

## 🗂 Структура практик

| 📌 Практика           | 📖 Что изучается                                                                         | ✅ Результат                                               |
| --------------------- | ---------------------------------------------------------------------------------------- | --------------------------------------------------------- |
| `01_basic_client`     | Базовое подключение через `pqxx::connection`, создание таблицы и транзакция `pqxx::work` | C++ программа подключается к PostgreSQL и создаёт таблицу |
| `02_query_results`    | Чтение результатов через `query_value`, `query`, `query1`, `query01`                     | Программа получает значения, строки и optional-результаты |
| `03_null_values`      | Обработка `NULL` через `std::optional`                                                   | Отсутствующие значения безопасно читаются в C++           |
| `04_sql_injection`    | Опасность SQL injection и защита через `quote`                                           | Пользовательский ввод безопасно вставляется в SQL         |
| `05_prepared_queries` | Подготовленные запросы через `prepare` и `exec_prepared`                                 | Значения передаются отдельно от SQL-шаблона               |

---

## 🔌 Подключение к PostgreSQL

| 📌 Параметр | 📖 Значение |
| ----------- | ----------- |
| Host        | `localhost` |
| Port        | `30432`     |
| User        | `postgres`  |
| Password    | `postgres`  |
| Database    | `postgres`  |

Строка подключения:

```text
postgres://postgres:postgres@localhost:30432/postgres
```

---

## 📚 Основные термины

| Термин             | Расшифровка                | Значение                                    |
| ------------------ | -------------------------- | ------------------------------------------- |
| `libpq`            | library Postgres Query     | C-библиотека клиента PostgreSQL             |
| `libpqxx`          | library Postgres Query C++ | C++-обёртка над `libpq`                     |
| `connection`       | соединение                 | Подключение к PostgreSQL                    |
| `work`             | работа / транзакция        | Транзакция для изменения данных             |
| `read_transaction` | транзакция чтения          | Транзакция только для чтения                |
| `exec`             | execute                    | Выполнить SQL                               |
| `query`            | запрос                     | Получить строки результата                  |
| `NULL`             | отсутствие значения        | Не `0` и не пустая строка                   |
| `optional`         | необязательное значение    | Значение может быть или отсутствовать       |
| `SQL injection`    | SQL-внедрение              | Атака через пользовательский ввод           |
| `prepared query`   | подготовленный запрос      | SQL-шаблон с отдельной передачей параметров |

---

## ⚙️ Общая сборка любой практики

| 📌 Шаг | 🧾 Команда                                                                 | 📖 Что делает                    |
| ------ | -------------------------------------------------------------------------- | -------------------------------- |
| 1      | `cd <папка_практики>`                                                      | Перейти в практику               |
| 2      | `mkdir build && cd build`                                                  | Создать папку сборки             |
| 3      | `conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11`      | Установить `libpqxx` через Conan |
| 4      | `cmake .. -DCMAKE_BUILD_TYPE=Debug`                                        | Сконфигурировать проект          |
| 5      | `cmake --build .`                                                          | Собрать программу                |
| 6      | `./bin/db_example "postgres://postgres:postgres@localhost:30432/postgres"` | Запустить пример                 |

Команды:

```bash
mkdir build
cd build

conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .

./bin/db_example "postgres://postgres:postgres@localhost:30432/postgres"
```

---

## 🧩 Практика 1 — `01_basic_client`

| 📌 Элемент                        | 📖 Что делает        | 🧠 Значение                         |
| --------------------------------- | -------------------- | ----------------------------------- |
| `#include <pqxx/pqxx>`            | Подключает `libpqxx` | Доступ к PostgreSQL из C++          |
| `pqxx::connection conn{argv[1]};` | Создаёт соединение   | Использует строку подключения       |
| `pqxx::work w(conn);`             | Открывает транзакцию | Нужна для изменения данных          |
| `w.exec(...)`                     | Выполняет SQL        | Создаёт таблицу `movies`            |
| `w.commit();`                     | Фиксирует изменения  | Без commit изменения не сохраняются |

Создаётся таблица:

```sql
CREATE TABLE IF NOT EXISTS movies (
    id SERIAL PRIMARY KEY,
    title varchar(200) NOT NULL,
    year integer NOT NULL
);
```

---

## 📖 Практика 2 — `02_query_results`

| Метод                               | Что ожидает          | Когда использовать                  |
| ----------------------------------- | -------------------- | ----------------------------------- |
| `query_value<double>`               | Одно значение        | `SELECT count(*)`, вычисления       |
| `query<int, std::string_view, int>` | Много строк          | Обычный список результатов          |
| `query1<int, std::string>`          | Ровно одну строку    | Когда результат обязан быть         |
| `query01<int, std::string>`         | Ноль или одну строку | Когда результат может отсутствовать |

Примеры:

```cpp
double value = r.query_value<double>("SELECT ||/100.;"_zv);

for (auto [id, title, year] : r.query<int, std::string_view, int>(query_text)) {
    std::cout << title << std::endl;
}

auto [id, title] = r.query1<int, std::string>(
    "SELECT id, title FROM movies WHERE year=2004 LIMIT 1;"_zv
);

std::optional result = r.query01<int, std::string>(
    "SELECT id, title FROM movies WHERE year=1999 LIMIT 1;"_zv
);
```

---

## ❓ Практика 3 — `03_null_values`

| SQL                   | C++                          |
| --------------------- | ---------------------------- |
| `NULL`                | `std::optional<T>`           |
| Значение есть         | `optional` содержит значение |
| Значения нет          | `optional` пустой            |
| Значение по умолчанию | `value_or(...)`              |

Пример:

```cpp
for (auto [x, y] : r.query<std::optional<int>, std::optional<int>>(
         "SELECT x, y FROM points;"_zv)) {
    std::cout << x.value_or(-9999) << ":" << y.value_or(-9999) << std::endl;
}
```

Ожидаемый вывод:

```text
-9999:10
20:-9999
```

---

## 🧨 Практика 4 — `04_sql_injection`

| Вариант                          | Что происходит              |
| -------------------------------- | --------------------------- |
| Склеивание строки SQL вручную    | Опасно                      |
| Пользовательский ввод внутри SQL | Возможна SQL injection      |
| `w.quote(title)`                 | Безопасное экранирование    |
| `--` в SQL                       | Комментарий до конца строки |

Опасная строка:

```text
xxx', 1); DROP TABLE movies; --
```

Правильно:

```cpp
w.exec(
    "INSERT INTO movies (title, year) VALUES (" +
    w.quote(title) +
    ", " +
    std::to_string(year) +
    ")"
);
```

`quote` сам добавляет кавычки и экранирует опасные символы.

---

## 🛡 Практика 5 — `05_prepared_queries`

| Элемент                | Что делает                      |
| ---------------------- | ------------------------------- |
| `conn.prepare(...)`    | Регистрирует SQL-шаблон         |
| `$1`, `$2`             | Плейсхолдеры параметров         |
| `w.exec_prepared(...)` | Выполняет подготовленный запрос |
| Значения               | Передаются отдельно от SQL      |

Пример:

```cpp
conn.prepare(
    tag_ins_movie,
    "INSERT INTO movies (title, year) VALUES ($1, $2)"_zv
);

w.exec_prepared(tag_ins_movie, title, year);
```

Главная идея:

```text
SQL-шаблон отдельно.
Данные отдельно.
```

Так безопаснее и удобнее.

---

## ✅ Главное запомнить

| Команда / класс          | Назначение                      |
| ------------------------ | ------------------------------- |
| `pqxx::connection`       | Соединение с PostgreSQL         |
| `pqxx::work`             | Транзакция для изменения данных |
| `pqxx::read_transaction` | Транзакция для чтения           |
| `exec`                   | Выполнить SQL                   |
| `commit`                 | Зафиксировать изменения         |
| `query_value`            | Получить одно значение          |
| `query`                  | Получить много строк            |
| `query1`                 | Получить ровно одну строку      |
| `query01`                | Получить ноль или одну строку   |
| `std::optional`          | Представить `NULL`              |
| `quote`                  | Безопасно экранировать значение |
| `prepare`                | Подготовить запрос              |
| `exec_prepared`          | Выполнить подготовленный запрос |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add lessons/sprint_20_20/postgres/lesson_6_8

git commit -m "Add libpqxx postgres lesson practices"

git push
```

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
