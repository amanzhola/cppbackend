# 📚 DB of Books

## 📌 Описание задачи

| 📌 Задача     | 📖 Что реализуется                                       | 🧠 Практический результат                                                                     |
| ------------- | -------------------------------------------------------- | --------------------------------------------------------------------------------------------- |
| `db_of_books` | Консольное C++ приложение для хранения книг в PostgreSQL | Реализован менеджер книг с добавлением записей, чтением всех книг и сохранением данных в базе |

---

## 🧠 Что используется

| Инструмент       | Назначение                |
| ---------------- | ------------------------- |
| PostgreSQL       | Хранение книг             |
| `libpqxx`        | C++ клиент PostgreSQL     |
| `boost::json`    | Разбор JSON-команд        |
| Conan            | Подключение зависимостей  |
| CMake            | Сборка проекта            |
| Prepared Queries | Безопасная вставка данных |
| `std::optional`  | Обработка `NULL` ISBN     |

---

## 📂 Структура решения

```text
solution/
├── CMakeLists.txt
├── conanfile.txt
├── main.cpp
└── README.md
```

---

## 📦 conanfile.txt

```ini
[requires]
libpqxx/7.7.4
boost/1.78.0

[generators]
cmake_multi
```

---

## ⚙️ CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.11)

project(book_manager CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(book_manager main.cpp)

target_link_libraries(book_manager
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
)
```

---

## 🗄 Таблица books

При запуске создаётся таблица:

```sql
CREATE TABLE IF NOT EXISTS books (
    id SERIAL PRIMARY KEY,
    title varchar(100) NOT NULL,
    author varchar(100) NOT NULL,
    year integer NOT NULL,
    isbn char(13) UNIQUE
);
```

| Поле     | Тип            | Ограничение                |
| -------- | -------------- | -------------------------- |
| `id`     | `SERIAL`       | `PRIMARY KEY`              |
| `title`  | `varchar(100)` | `NOT NULL`                 |
| `author` | `varchar(100)` | `NOT NULL`                 |
| `year`   | `integer`      | `NOT NULL`                 |
| `isbn`   | `char(13)`     | `UNIQUE`, допускает `NULL` |

---

## 🔐 Prepared Queries

Используются два подготовленных запроса:

| Запрос                     | Назначение              |
| -------------------------- | ----------------------- |
| `insert_book_with_isbn`    | Добавить книгу с ISBN   |
| `insert_book_without_isbn` | Добавить книгу без ISBN |

```cpp
conn.prepare(
    kInsertBookWithIsbn,
    "INSERT INTO books (title, author, year, isbn) VALUES ($1, $2, $3, $4)"_zv
);
```

```cpp
conn.prepare(
    kInsertBookWithoutIsbn,
    "INSERT INTO books (title, author, year, isbn) VALUES ($1, $2, $3, NULL)"_zv
);
```

---

## ➕ Добавление книги

Команда приходит в формате JSON:

```json
{
  "action": "add_book",
  "payload": {
    "title": "The Old Man and the Sea",
    "author": "Hemingway",
    "year": 1952,
    "ISBN": "5555555555555"
  }
}
```

Если книга добавлена успешно:

```json
{"result":true}
```

Если нарушено ограничение базы, например повторный ISBN:

```json
{"result":false}
```

---

## 📖 Добавление книги без ISBN

```json
{
  "action": "add_book",
  "payload": {
    "title": "Anna Karenina",
    "author": "Tolstoi",
    "year": 1878,
    "ISBN": null
  }
}
```

В этом случае в PostgreSQL сохраняется:

```sql
isbn = NULL
```

---

## 📚 Получение всех книг

Команда:

```json
{"action":"all_books","payload":{}}
```

SQL-запрос:

```sql
SELECT id, title, author, year, isbn
FROM books
ORDER BY year DESC, title ASC, author ASC, isbn ASC
```

Сортировка:

| Поле     | Порядок |
| -------- | ------- |
| `year`   | DESC    |
| `title`  | ASC     |
| `author` | ASC     |
| `isbn`   | ASC     |

---

## 🧩 Обработка NULL ISBN

В C++ используется:

```cpp
std::optional<std::string>
```

Если `isbn` есть:

```cpp
book["ISBN"] = *isbn;
```

Если `isbn` отсутствует:

```cpp
book["ISBN"] = nullptr;
```

---

## 🛠 Сборка

```bash
cd ~/cppbackend/sprint4/problems/db_of_books/solution

rm -rf build
mkdir build
cd build

conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

---

## ▶️ Запуск

```bash
./bin/book_manager "postgres://postgres:postgres@localhost:30432/postgres"
```

---

## 🧪 Пример ручного теста

Ввести построчно:

```json
{"action":"add_book","payload":{"title":"The Old Man and the Sea","author":"Hemingway","year":1952,"ISBN":"5555555555555"}}
{"action":"add_book","payload":{"title":"Anna Karenina","author":"Tolstoi","year":1878,"ISBN":null}}
{"action":"all_books","payload":{}}
{"action":"exit","payload":{}}
```

---

## ✅ Почему решение соответствует требованиям

| Требование                    | Как выполнено                                         |
| ----------------------------- | ----------------------------------------------------- |
| Таблица создаётся при запуске | `CREATE TABLE IF NOT EXISTS`                          |
| Уникальный id                 | `SERIAL PRIMARY KEY`                                  |
| Обязательные поля             | `NOT NULL`                                            |
| Уникальный ISBN               | `isbn char(13) UNIQUE`                                |
| ISBN может быть `NULL`        | Поле `isbn` не `NOT NULL`                             |
| Защита от SQL injection       | `exec_prepared`                                       |
| Чтение книг                   | `pqxx::read_transaction`                              |
| Сортировка результата         | `ORDER BY year DESC, title ASC, author ASC, isbn ASC` |
| JSON-ответы                   | `boost::json::serialize`                              |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add sprint4/problems/db_of_books/solution

git commit -m "Implement book manager database task"

git push
```

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../../../../lessons/sprint_20_20/postgres/README.md)

[Вернуться в общий README репозитория](../../../../README.md)

