# 📚 Bookypedia-2

## 📌 Путь решения

```text
sprint4/problems/bookypedia-2/solution
```

Решение построено на основе предыдущего задания:

```text
sprint4/problems/bookypedia-1/solution
```

---

## Подготовка решения

В качестве основы использовалось решение предыдущего этапа.

Команды:

```bash
cd ~/cppbackend

rm -rf sprint4/problems/bookypedia-2/solution/*

cp -a sprint4/problems/bookypedia-1/solution/. \
      sprint4/problems/bookypedia-2/solution/

rm -rf sprint4/problems/bookypedia-2/solution/build
```

---

## 📌 Цель задания

Требовалось расширить приложение Bookypedia новыми возможностями работы с книгами и авторами.

Необходимо было:

```text
• добавить поддержку тегов книг;
• реализовать просмотр полной информации о книге;
• реализовать редактирование книг и авторов;
• реализовать удаление книг и авторов;
• обеспечить корректное хранение тегов в PostgreSQL;
• сохранить существующую многослойную архитектуру проекта.
```

---

## 🏗 Архитектура проекта

Структура приложения сохранена без упрощений.

Используются отдельные слои:

```text
domain
app
postgres
ui
menu
main
```

Назначение слоёв:

```text
domain
    ↓
доменная модель и интерфейсы репозиториев

app
    ↓
Use Cases приложения

postgres
    ↓
реализация репозиториев через PostgreSQL/libpqxx

ui
    ↓
консольный пользовательский интерфейс

menu
    ↓
маршрутизация пользовательских команд

main
    ↓
точка запуска приложения
```

---

## 📂 Изменённые файлы

```text
src/domain/author.h

src/app/use_cases.h
src/app/use_cases_impl.h
src/app/use_cases_impl.cpp

src/postgres/postgres.h
src/postgres/postgres.cpp

src/ui/view.h
src/ui/view.cpp

src/bookypedia.h
src/bookypedia.cpp
```

---

## 🗄 Структура базы данных

При запуске приложение автоматически создаёт необходимые таблицы.

---

### Таблица authors

```sql
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
```

Назначение:

```text
Хранение авторов книг.
```

---

### Таблица books

```sql
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL
        REFERENCES authors(id)
        ON DELETE CASCADE,

    title varchar(100) NOT NULL,
    publication_year integer NOT NULL
);
```

Назначение:

```text
Хранение книг.
```

---

### Таблица book_tags

```sql
CREATE TABLE IF NOT EXISTS book_tags (
    book_id UUID NOT NULL
        REFERENCES books(id)
        ON DELETE CASCADE,

    tag varchar(30) NOT NULL,

    PRIMARY KEY (book_id, tag)
);
```

Назначение:

```text
Хранение тегов книг.
```

---

## Что даёт ON DELETE CASCADE

Используется связь:

```sql
ON DELETE CASCADE
```

Смысл:

```text
Удалили автора
        ↓
Удалились его книги
        ↓
Удалились связанные теги
```

Аналогично:

```text
Удалили книгу
        ↓
Удалились её теги
```

Это предотвращает появление "висячих" записей.

---

## 📚 Новые возможности

Добавлены команды:

```text
AddBook
ShowBooks
ShowBook
DeleteBook
DeleteAuthor
EditAuthor
EditBook
```

---

### AddBook

Добавляет новую книгу.

Дополнительно поддерживает:

```text
автоматическое создание автора;
ввод списка тегов.
```

---

### ShowBooks

Отображает список книг.

---

### ShowBook

Показывает подробную информацию о книге:

```text
Название
Автор
Год издания
Теги
```

---

### EditBook

Редактирование книги.

Позволяет изменять:

```text
название
год издания
теги
```

---

### EditAuthor

Редактирование имени автора.

---

### DeleteBook

Удаляет книгу.

---

### DeleteAuthor

Удаляет автора.

Благодаря:

```sql
ON DELETE CASCADE
```

автоматически удаляются:

```text
книги автора
теги этих книг
```

---

## 🧪 Ручная проверка

### Очистка базы

Подключение:

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-d postgres
```

Удаление таблиц:

```sql
DROP TABLE IF EXISTS book_tags;
DROP TABLE IF EXISTS books;
DROP TABLE IF EXISTS authors;

\q
```

---

### Запуск приложения

```bash
cd ~/cppbackend/sprint4/problems/bookypedia-2/solution/build

export BOOKYPEDIA_DB_URL="postgres://postgres:postgres@localhost:30432/postgres"

./bookypedia
```

---

### Тестовый сценарий

Ввод:

```text
AddBook 1906 White Fang

Jack London

y

adventure, dog,   gold   rush  ,  dog,,dogs

ShowBooks

ShowBook White Fang

Exit
```

---

### Ожидаемый результат

Смысловой вывод:

```text
Enter author name or empty line to select from list:

No author found. Do you want to add Jack London (y/n)?

Enter tags (comma separated):

1 White Fang by Jack London, 1906

Title: White Fang
Author: Jack London
Publication year: 1906
Tags: adventure, dog, dogs, gold rush
```

---

## ⚙️ Сборка проекта

```bash
cd ~/cppbackend/sprint4/problems/bookypedia-2/solution

rm -rf build

mkdir build

cd build

conan install .. \
    -s build_type=Debug \
    -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

---

## Проверенный результат сборки

```text
[100%] Built target bookypedia
[100%] Built target tests
```

---

## 📌 Что было изучено

| Тема                     | Результат |
| ------------------------ | --------- |
| PostgreSQL               | ✅         |
| UUID                     | ✅         |
| ON DELETE CASCADE        | ✅         |
| Repository Pattern       | ✅         |
| Use Cases                | ✅         |
| Работа с тегами          | ✅         |
| CRUD-операции            | ✅         |
| libpqxx                  | ✅         |
| Многослойная архитектура | ✅         |

---

## 🏁 Итог

В ходе выполнения задания приложение Bookypedia было расширено поддержкой тегов книг, просмотром полной информации о книге, редактированием и удалением сущностей.

Сохранена архитектура проекта с разделением на доменную модель, слой сценариев использования, PostgreSQL-репозитории и пользовательский интерфейс.

Дополнительно получен практический опыт проектирования связей между таблицами PostgreSQL, использования UUID-идентификаторов, каскадного удаления данных и реализации CRUD-операций поверх Repository Pattern.

## ⬅️ Назад

[Вернуться к Keys, Indexes and Transactions](../../../lessons/sprint_20_20/keys_indexes_transactions/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
