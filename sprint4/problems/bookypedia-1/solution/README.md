# 📚 Bookypedia-1

## 📌 Описание задания

| 📌 Задача      | 📖 Что требовалось реализовать                                 | 🧠 Практическая цель                                                           | ✅ Результат                                                        |
| -------------- | -------------------------------------------------------------- | ------------------------------------------------------------------------------ | ------------------------------------------------------------------ |
| `bookypedia-1` | Консольное приложение для хранения авторов и книг в PostgreSQL | Закрепить паттерн Repository, `libpqxx`, UUID, use cases и работу с PostgreSQL | Реализованы авторы, книги, репозитории, UI-команды и хранение в БД |

---

## 📂 Пути

| 📌 Каталог                                            | 📖 Назначение     |
| ----------------------------------------------------- | ----------------- |
| `~/cppbackend/sprint4/problems/bookypedia-1/precode`  | Заготовка задания |
| `~/cppbackend/sprint4/problems/bookypedia-1/solution` | Итоговое решение  |

Подготовка решения:

```bash
cd ~/cppbackend

rm -rf sprint4/problems/bookypedia-1/solution/*

cp -a sprint4/problems/bookypedia-1/precode/. sprint4/problems/bookypedia-1/solution/
```

---

## 🔌 Подключение к базе

| 📌 Переменная       | 📖 Значение                  |
| ------------------- | ---------------------------- |
| `BOOKYPEDIA_DB_URL` | URL подключения к PostgreSQL |

Пример:

```bash
export BOOKYPEDIA_DB_URL="postgres://postgres:postgres@localhost:30432/postgres"
```

---

## 🗄 Таблица authors

```sql
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
```

| Поле   | Назначение                            |
| ------ | ------------------------------------- |
| `id`   | UUID автора                           |
| `name` | Имя автора, уникальное и обязательное |

---

## 📘 Таблица books

```sql
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL REFERENCES authors(id),
    title varchar(100) NOT NULL,
    publication_year integer NOT NULL
);
```

| Поле               | Назначение       |
| ------------------ | ---------------- |
| `id`               | UUID книги       |
| `author_id`        | Ссылка на автора |
| `title`            | Название книги   |
| `publication_year` | Год публикации   |

---

## 🧱 Архитектура

| Слой       | Что содержит                              |
| ---------- | ----------------------------------------- |
| `domain`   | `Author`, `Book`, интерфейсы репозиториев |
| `app`      | `UseCases`, бизнес-сценарии               |
| `postgres` | Реализация репозиториев через PostgreSQL  |
| `ui`       | Команды консольного интерфейса            |

Схема:

```text
ui
↓
app
↓
domain interfaces
↓
postgres repositories
↓
PostgreSQL
```

---

## 📄 Изменённые файлы

| Файл                         | Что сделано                                                                 |
| ---------------------------- | --------------------------------------------------------------------------- |
| `src/domain/author.h`        | Добавлены `Book`, `BookId`, `BookRepository`, расширен `AuthorRepository`   |
| `src/app/use_cases.h`        | Добавлены `AddBook`, `GetBooks`, `GetAuthorBooks`, `AuthorInfo`, `BookInfo` |
| `src/app/use_cases_impl.h`   | `UseCasesImpl` работает с авторами и книгами                                |
| `src/app/use_cases_impl.cpp` | Реализованы сценарии авторов и книг                                         |
| `src/postgres/postgres.h`    | Добавлены `AuthorRepositoryImpl`, `BookRepositoryImpl`, `Database`          |
| `src/postgres/postgres.cpp`  | Реализована работа с PostgreSQL                                             |
| `src/bookypedia.h`           | `UseCasesImpl` создаётся с двумя репозиториями                              |
| `src/ui/view.cpp`            | Реализованы команды UI                                                      |

---

## 🧠 Доменный слой

| Объект             | Назначение                 |
| ------------------ | -------------------------- |
| `Author`           | Автор книги                |
| `Book`             | Книга                      |
| `AuthorRepository` | Интерфейс хранения авторов |
| `BookRepository`   | Интерфейс хранения книг    |

Главная идея:

```text
Доменный слой не знает про PostgreSQL.
```

---

## 🧩 Use Cases

| Метод            | Назначение                       |
| ---------------- | -------------------------------- |
| `AddAuthor`      | Добавить автора                  |
| `GetAuthors`     | Получить список авторов          |
| `AddBook`        | Добавить книгу                   |
| `GetBooks`       | Получить все книги               |
| `GetAuthorBooks` | Получить книги выбранного автора |

---

## 🐘 PostgreSQL-слой

| Класс                  | Назначение                      |
| ---------------------- | ------------------------------- |
| `AuthorRepositoryImpl` | Сохраняет и читает авторов      |
| `BookRepositoryImpl`   | Сохраняет и читает книги        |
| `Database`             | Хранит соединение и репозитории |

Операции выполняются через:

```cpp
pqxx::work
```

для записи и:

```cpp
pqxx::read_transaction
```

для чтения.

---

## 🖥 UI-команды

| Команда           | Что делает                       |
| ----------------- | -------------------------------- |
| `AddAuthor`       | Добавить автора                  |
| `ShowAuthors`     | Показать авторов                 |
| `AddBook`         | Добавить книгу                   |
| `ShowBooks`       | Показать книги                   |
| `ShowAuthorBooks` | Показать книги выбранного автора |
| `Exit`            | Выйти                            |

---

## 🧯 Проблемы и решения

| Проблема                                   | Причина                                                       | Решение                                                     |
| ------------------------------------------ | ------------------------------------------------------------- | ----------------------------------------------------------- |
| `catch2/3.2.0 build failed`                | Старая версия Catch2 плохо собиралась с GCC 13 / Ubuntu 24.04 | Заменить на `catch2/3.3.2`                                  |
| `Missing prebuilt package`                 | Conan не нашёл готовые бинарные пакеты                        | Добавить `--build=missing`                                  |
| `compiler.libcxx=libstdc++`                | Старый ABI в Conan profile                                    | Использовать `libstdc++11`                                  |
| Папка `build/build`                        | `conan install ..` запускался не из той папки                 | Пересоздать `solution/build`                                |
| `pqxx::query` ломался                      | Проблема связки `libpqxx 7.7.4 + GCC 13`                      | Использовать `read.exec(...)` и ручное чтение строк         |
| Старые unit-тесты ожидали старый интерфейс | `UseCasesImpl` раньше принимал только `AuthorRepository`      | Добавить совместимый конструктор                            |
| `connection refused`                       | PostgreSQL-контейнер остановлен                               | `sudo docker start postgres16`                              |
| `column "author_id" does not exist`        | В базе осталась старая таблица `books`                        | `DROP TABLE IF EXISTS books; DROP TABLE IF EXISTS authors;` |
| Неверная команда выхода                    | Для `psql` и `bookypedia` разные команды                      | `\q` для `psql`, `Exit` для `bookypedia`                    |

---

## ⚙️ Правильная сборка

```bash
cd ~/cppbackend/sprint4/problems/bookypedia-1/solution

rm -rf build
mkdir build
cd build

conan install .. \
  --build=missing \
  -s build_type=Debug \
  -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

---

## ▶️ Правильный запуск

Запустить PostgreSQL:

```bash
sudo docker start postgres16
```

Задать переменную окружения:

```bash
export BOOKYPEDIA_DB_URL="postgres://postgres:postgres@localhost:30432/postgres"
```

Запустить приложение:

```bash
cd ~/cppbackend/sprint4/problems/bookypedia-1/solution/build

./bookypedia
```

---

## 🧪 Проверочный сценарий

```text
AddAuthor Joanne Rowling
AddAuthor Jack London
ShowAuthors
AddBook 1998 Harry Potter and the Chamber of Secrets
2
ShowBooks
ShowAuthorBooks
2
Exit
```

---

## ✅ Ожидаемый результат

```text
1 Jack London
2 Joanne Rowling
Select author:
1 Jack London
2 Joanne Rowling
Enter author # or empty line to cancel
1 Harry Potter and the Chamber of Secrets, 1998
Select author:
1 Jack London
2 Joanne Rowling
Enter author # or empty line to cancel
1 Harry Potter and the Chamber of Secrets, 1998
```

---

## 🏁 Итог

| Что реализовано                        | Статус |
| -------------------------------------- | ------ |
| `AddAuthor`                            | ✅      |
| `ShowAuthors`                          | ✅      |
| `AddBook`                              | ✅      |
| `ShowBooks`                            | ✅      |
| `ShowAuthorBooks`                      | ✅      |
| Хранение авторов в PostgreSQL          | ✅      |
| Хранение книг в PostgreSQL             | ✅      |
| Паттерн Repository                     | ✅      |
| UUID для авторов и книг                | ✅      |
| Слои `domain`, `app`, `postgres`, `ui` | ✅      |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add sprint4/problems/bookypedia-1/solution

git commit -m "Implement bookypedia repository solution"

git push
```

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../../../../lessons/sprint_20_20/postgres/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
