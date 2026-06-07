# 🐘 PostgreSQL — Lesson 5/8

## 📌 Тема урока

| 📌 Тема                   | 📖 Что изучается                                                                                    | 🧠 Практический результат                                                                           |
| ------------------------- | --------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| Типы данных и ограничения | `ENUM`, `SERIAL`, `PRIMARY KEY`, `NOT NULL`, `DEFAULT`, `CHECK`, `UNIQUE`, именованные `CONSTRAINT` | Освоено создание строгой схемы данных, где PostgreSQL сам защищает таблицу от некорректных значений |

---

## 🗂 Структура практики

| 📌 Практика | 📖 Что делается                                    | ✅ Результат                                  |
| ----------- | -------------------------------------------------- | -------------------------------------------- |
| 1           | Создание пользовательского `ENUM`-типа `gear_type` | Появляется тип шин                           |
| 2           | Создание таблицы `gears`                           | Таблица содержит ограничения                 |
| 3           | Именованные ограничения                            | Ошибки становятся понятнее                   |
| 4           | Корректные вставки                                 | Валидные данные сохраняются                  |
| 5           | Проверка `CHECK`                                   | Некорректный вес запрещён                    |
| 6           | Проверка `UNIQUE`                                  | Дублирующий `serial` запрещён                |
| 7           | Проверка `DEFAULT`                                 | Тип по умолчанию подставляется автоматически |
| 8           | Проверка `NOT NULL`                                | Обязательные поля нельзя пропускать          |
| 9           | Исследование структуры                             | Проверяются таблицы, типы и данные           |
| 10          | Очистка                                            | Удаляются таблица, тип и база                |

---

## 🔌 Подключение к PostgreSQL

| 📌 Команда                                  | 📖 Назначение                      |
| ------------------------------------------- | ---------------------------------- |
| `psql -h localhost -p 30432 -U postgres -W` | Подключиться к PostgreSQL в Docker |
| `postgres`                                  | Пароль пользователя `postgres`     |

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-W
```

---

## 🗄 Создание базы

| 📌 SQL                           | 📖 Что делает                    |
| -------------------------------- | -------------------------------- |
| `CREATE DATABASE lesson_5_8_db;` | Создаёт отдельную базу для урока |
| `\c lesson_5_8_db`               | Подключается к базе              |

```sql
CREATE DATABASE lesson_5_8_db;

\c lesson_5_8_db
```

---

## 🧩 Практика 1 — ENUM-тип

| 📌 Элемент    | 📖 Значение                      |
| ------------- | -------------------------------- |
| `CREATE TYPE` | Создать пользовательский тип     |
| `gear_type`   | Имя типа                         |
| `AS ENUM`     | Перечисление допустимых значений |
| `winter`      | Зимний тип                       |
| `summer`      | Летний тип                       |
| `demiseason`  | Демисезонный тип                 |

```sql
CREATE TYPE gear_type AS ENUM (
    'winter',
    'summer',
    'demiseason'
);
```

Проверка:

```sql
\dT
```

Комментарий к типу:

```sql
COMMENT ON TYPE gear_type
IS 'тип шин';
```

---

## ⚙️ Практика 2 — таблица `gears`

```sql
CREATE TABLE gears (
    id              SERIAL PRIMARY KEY,
    firm            varchar(40) NOT NULL,
    type            gear_type NOT NULL DEFAULT('demiseason'),
    bearing_weight  integer CHECK (bearing_weight > 0),
    pressure        integer CHECK (pressure > 0),
    serial          varchar(10) UNIQUE NOT NULL
);
```

| Поле             | Значение                     |
| ---------------- | ---------------------------- |
| `id`             | Автоматический идентификатор |
| `firm`           | Производитель                |
| `type`           | Тип шин                      |
| `bearing_weight` | Допустимая нагрузка          |
| `pressure`       | Давление                     |
| `serial`         | Уникальный серийный номер    |

Проверка:

```sql
\d gears
```

---

## 🔢 SERIAL

| 📌 Термин     | 📖 Значение                    |
| ------------- | ------------------------------ |
| `SERIAL`      | Автоувеличивающийся integer    |
| `sequence`    | Последовательность значений    |
| `PRIMARY KEY` | Уникальный и обязательный ключ |

`SERIAL` можно представить как:

```cpp
static int next_id = 1;

obj.id = next_id++;
```

---

## 🧱 Основные ограничения

| Ограничение   | Что делает                        |
| ------------- | --------------------------------- |
| `PRIMARY KEY` | Уникальный ключ, не `NULL`        |
| `NOT NULL`    | Запрещает отсутствие значения     |
| `DEFAULT`     | Подставляет значение по умолчанию |
| `CHECK`       | Проверяет условие                 |
| `UNIQUE`      | Запрещает дубли                   |

---

## 🏷 Практика 3 — именованные ограничения

Сначала удалить старую таблицу:

```sql
DROP TABLE gears;
```

Создать таблицу с именованными ограничениями:

```sql
CREATE TABLE gears (
    id SERIAL
        CONSTRAINT main_key
        PRIMARY KEY,

    firm varchar(40)
        NOT NULL,

    type gear_type
        NOT NULL
        DEFAULT('demiseason'),

    bearing_weight integer
        CONSTRAINT weight_positive
        CHECK (bearing_weight > 0),

    pressure integer
        CONSTRAINT pressure_positive
        CHECK (pressure > 0),

    serial varchar(10)
        CONSTRAINT serial_unique
        UNIQUE
        NOT NULL
);
```

Проверка:

```sql
\d gears
```

Теперь ошибки и описание таблицы используют имена:

```text
main_key
weight_positive
pressure_positive
serial_unique
```

---

## ✅ Практика 4 — корректные вставки

```sql
INSERT INTO gears
(
    firm,
    type,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'Michelin',
    'winter',
    1200,
    32,
    'M001'
);
```

```sql
INSERT INTO gears
(
    firm,
    type,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'Bridgestone',
    'summer',
    1000,
    30,
    'B001'
);
```

Проверка:

```sql
SELECT * FROM gears;
```

---

## ❌ Практика 5 — проверка CHECK

Некорректная вставка:

```sql
INSERT INTO gears
(
    firm,
    type,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'BadTyre',
    'winter',
    -100,
    30,
    'BAD01'
);
```

Ожидается ошибка ограничения:

```text
weight_positive
```

Причина:

```text
bearing_weight > 0
```

---

## ❌ Практика 6 — проверка UNIQUE

Дублирующий `serial`:

```sql
INSERT INTO gears
(
    firm,
    type,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'Michelin',
    'winter',
    1200,
    32,
    'M001'
);
```

Ожидается ошибка:

```text
serial_unique
```

---

## ✅ Практика 7 — проверка DEFAULT

Вставка без `type`:

```sql
INSERT INTO gears
(
    firm,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'Goodyear',
    900,
    28,
    'G001'
);
```

Проверка:

```sql
SELECT * FROM gears;
```

Ожидается:

```text
type = demiseason
```

---

## ❌ Практика 8 — проверка NOT NULL

Вставка без `firm`:

```sql
INSERT INTO gears
(
    type,
    bearing_weight,
    pressure,
    serial
)
VALUES
(
    'winter',
    1000,
    30,
    'ERR01'
);
```

Ожидается ошибка:

```text
firm violates not-null constraint
```

---

## 🔍 Практика 9 — исследование структуры

| Команда                | Назначение                   |
| ---------------------- | ---------------------------- |
| `\dt`                  | Посмотреть таблицы           |
| `\dT`                  | Посмотреть типы              |
| `\d gears`             | Посмотреть структуру таблицы |
| `SELECT * FROM gears;` | Посмотреть данные            |

```sql
\dt

\dT

\d gears

SELECT * FROM gears;
```

---

## 🧹 Практика 10 — очистка

```sql
DROP TABLE gears;

DROP TYPE gear_type;

\dT

\c postgres

DROP DATABASE lesson_5_8_db;
```

---

## ✅ Что изучено

| SQL-конструкция   | Назначение                       |
| ----------------- | -------------------------------- |
| `CREATE TYPE`     | Создать пользовательский тип     |
| `ENUM`            | Перечисление допустимых значений |
| `COMMENT ON TYPE` | Добавить описание к типу         |
| `CREATE TABLE`    | Создать таблицу                  |
| `SERIAL`          | Автоматический идентификатор     |
| `PRIMARY KEY`     | Главный ключ                     |
| `NOT NULL`        | Обязательное значение            |
| `DEFAULT`         | Значение по умолчанию            |
| `CHECK`           | Проверка условия                 |
| `UNIQUE`          | Уникальность                     |
| `CONSTRAINT`      | Именованное ограничение          |
| `INSERT`          | Добавление данных                |
| `SELECT`          | Чтение данных                    |
| `DROP TABLE`      | Удаление таблицы                 |
| `DROP TYPE`       | Удаление типа                    |
| `DROP DATABASE`   | Удаление базы                    |

---

## 🏁 Итог

| 📌 Что сделано                    | 📖 Почему это важно                                   |
| --------------------------------- | ----------------------------------------------------- |
| Создан `ENUM`                     | Значение поля ограничено списком допустимых вариантов |
| Создана таблица `gears`           | Таблица содержит не только данные, но и правила       |
| Добавлены `CHECK`                 | Нельзя сохранить отрицательные значения               |
| Добавлен `UNIQUE`                 | Нельзя повторить серийный номер                       |
| Добавлен `DEFAULT`                | Значение подставляется автоматически                  |
| Добавлены именованные ограничения | Ошибки становятся понятнее                            |

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
