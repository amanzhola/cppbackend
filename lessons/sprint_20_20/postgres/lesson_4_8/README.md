# 🐘 PostgreSQL — Lesson 4/8

## 📌 Тема урока

| 📌 Тема  | 📖 Что изучается                                                                                                                                 | 🧠 Практический результат                                        |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------- |
| Язык SQL | `CREATE DATABASE`, `DROP DATABASE`, `CREATE TABLE`, `INSERT`, `SELECT`, `WHERE`, `ORDER BY`, `LIMIT`, `OFFSET`, `UPDATE`, `DELETE`, `DROP TABLE` | Освоены базовые SQL-команды для работы с PostgreSQL через `psql` |

---

## 🔌 Подключение к PostgreSQL

| 📌 Команда / параметр                       | 📖 Значение              | 🧠 Пояснение                                                   |
| ------------------------------------------- | ------------------------ | -------------------------------------------------------------- |
| `psql -h localhost -p 30432 -U postgres -W` | Подключение к PostgreSQL | Сервер PostgreSQL работает в Docker, а подключение идёт из WSL |
| `-h localhost`                              | Host                     | Локальный адрес `127.0.0.1`                                    |
| `-p 30432`                                  | Port                     | Внешний порт, проброшенный в Docker                            |
| `-U postgres`                               | User                     | Пользователь PostgreSQL                                        |
| `-W`                                        | Password                 | Запросить пароль                                               |

```bash
psql -h localhost -p 30432 -U postgres -W
```

Пароль:

```text
postgres
```

---

## 🗄 Работа с базой данных

| 📌 SQL-команда                 | 📖 Что делает                | ✅ Результат                            |
| ------------------------------ | ---------------------------- | -------------------------------------- |
| `CREATE DATABASE yandex_test;` | Создаёт новую базу данных    | Появляется база `yandex_test`          |
| `\l`                           | Показывает список баз данных | Можно проверить, что база создана      |
| `\c yandex_test`               | Подключается к базе          | Активной становится база `yandex_test` |
| `DROP DATABASE yandex_test;`   | Удаляет базу данных          | База удаляется полностью               |

```sql
CREATE DATABASE yandex_test;

\l

\c yandex_test
```

---

## 🧮 Вычисления и литералы

| 📌 SQL                                          | 📖 Что делает                            | ✅ Результат                       |
| ----------------------------------------------- | ---------------------------------------- | --------------------------------- |
| `SELECT current_database();`                    | Показывает текущую базу данных           | Возвращает `yandex_test`          |
| `SELECT 2 + 2;`                                 | Выполняет вычисление                     | Возвращает `4`                    |
| `SELECT 10 * 5 AS result;`                      | Вычисляет выражение и задаёт имя столбца | Возвращает столбец `result`       |
| `SELECT length('Hello, Practicum');`            | Считает длину строки                     | Возвращает длину строки           |
| `SELECT 'User''s name';`                        | Показывает строку с апострофом           | Апостроф экранируется двойным `'` |
| `SELECT E'Hello, \'Postgres\'!\nHello, user!';` | Использует escape-строку                 | Позволяет применять `\n`, `\'`    |

```sql
SELECT current_database();

SELECT 2 + 2;

SELECT 10 * 5 AS result;

SELECT length('Hello, Practicum');

SELECT 'User''s name';

SELECT E'Hello, \'Postgres\'!\nHello, user!';
```

---

## 📐 Создание таблицы

| 📌 Элемент     | 📖 Значение     | 🧠 Пояснение                         |
| -------------- | --------------- | ------------------------------------ |
| `CREATE TABLE` | Создать таблицу | Создаёт структуру для хранения строк |
| `points`       | Имя таблицы     | Таблица точек                        |
| `x integer`    | Столбец `x`     | Целое число                          |
| `y integer`    | Столбец `y`     | Целое число                          |
| `\dt`          | Display tables  | Показать таблицы                     |
| `\d points`    | Describe        | Показать структуру таблицы           |

```sql
CREATE TABLE points (
    x integer,
    y integer
);

\dt

\d points
```

---

## ➕ INSERT — добавление строк

| 📌 Команда                                          | 📖 Что делает                    | ✅ Результат                         |
| --------------------------------------------------- | -------------------------------- | ----------------------------------- |
| `INSERT INTO points VALUES (10, -20);`              | Добавляет одну строку            | В таблице появляется точка          |
| `INSERT INTO points VALUES (0, 0), (1, 1), (2, 2);` | Добавляет несколько строк        | Вставляется сразу несколько точек   |
| `INSERT INTO points (y) VALUES (50);`               | Заполняет только `y`             | `x` получает `NULL`                 |
| `INSERT INTO points VALUES (100, DEFAULT);`         | Использует значение по умолчанию | Если default не задан, будет `NULL` |

```sql
INSERT INTO points VALUES (10, -20);

INSERT INTO points VALUES
    (0, 0),
    (1, 1),
    (2, 2);

INSERT INTO points (y) VALUES (50);

INSERT INTO points VALUES (100, DEFAULT);
```

---

## 🔍 SELECT — чтение данных

| 📌 Запрос                                               | 📖 Что делает                 | ✅ Результат                  |
| ------------------------------------------------------- | ----------------------------- | ---------------------------- |
| `SELECT * FROM points;`                                 | Выбирает все строки и столбцы | Показывает всю таблицу       |
| `SELECT x FROM points;`                                 | Выбирает только `x`           | Показывает один столбец      |
| `SELECT x, y, x + y AS sum FROM points;`                | Добавляет вычисляемый столбец | Показывает сумму координат   |
| `SELECT x, y, @x + @y AS abs_sum FROM points;`          | Использует модуль чисел       | Показывает сумму модулей     |
| `SELECT x, y, \|/(x*x + y*y) AS dist_to_0 FROM points;` | Считает расстояние до нуля    | Использует квадратный корень |

```sql
SELECT * FROM points;

SELECT x FROM points;

SELECT x, y, x + y AS sum FROM points;

SELECT x, y, @x + @y AS abs_sum FROM points;

SELECT x, y, |/(x*x + y*y) AS dist_to_0 FROM points;
```

---

## ❓ NULL

| 📌 Термин     | 📖 Значение                     | 🧠 Важно                           |
| ------------- | ------------------------------- | ---------------------------------- |
| `NULL`        | Отсутствие значения             | Это не `0` и не пустая строка      |
| `IS NULL`     | Проверка на отсутствие значения | Используется в `WHERE`             |
| `IS NOT NULL` | Проверка на наличие значения    | Используется в фильтрации          |
| `DEFAULT`     | Значение по умолчанию           | Если не задано, может стать `NULL` |

---

## 🧹 DELETE и WHERE

| 📌 Команда                                         | 📖 Что делает                            | ✅ Результат                        |
| -------------------------------------------------- | ---------------------------------------- | ---------------------------------- |
| `DELETE FROM points WHERE x IS NULL OR y IS NULL;` | Удаляет строки с пустыми координатами    | Остаются только полные точки       |
| `SELECT * FROM points WHERE x > 0 AND y > 0;`      | Показывает точки в положительной области | Работают условия `AND`             |
| `DELETE FROM points;`                              | Удаляет все строки                       | Таблица остаётся, данные удаляются |

```sql
DELETE FROM points
WHERE x IS NULL OR y IS NULL;

SELECT * FROM points
WHERE x > 0 AND y > 0;

DELETE FROM points;
```

---

## 📊 ORDER BY — сортировка

| 📌 Команда                | 📖 Что делает              | ✅ Результат            |
| ------------------------- | -------------------------- | ---------------------- |
| `ORDER BY x`              | Сортирует по `x`           | По возрастанию         |
| `ORDER BY x DESC`         | Сортирует по `x`           | По убыванию            |
| `ORDER BY x ASC`          | Сортирует по `x`           | По возрастанию явно    |
| `ORDER BY x DESC, y DESC` | Сортирует по двум столбцам | Сначала `x`, затем `y` |

```sql
INSERT INTO points VALUES
    (10, -20),
    (0, 0),
    (1, 1),
    (1, 2),
    (DEFAULT, 50),
    (100, DEFAULT);

SELECT * FROM points ORDER BY x;

SELECT * FROM points ORDER BY x DESC;

SELECT * FROM points ORDER BY x ASC;

SELECT * FROM points ORDER BY x DESC, y DESC;
```

---

## 📄 LIMIT и OFFSET

| 📌 Команда                    | 📖 Что делает                             | ✅ Результат                           |
| ----------------------------- | ----------------------------------------- | ------------------------------------- |
| `LIMIT 3`                     | Ограничивает количество строк             | Показывает только 3 строки            |
| `OFFSET 2`                    | Пропускает первые 2 строки                | Начинает вывод с третьей строки       |
| `ORDER BY x LIMIT 3 OFFSET 2` | Сортирует, ограничивает и делает смещение | Используется для постраничного вывода |

```sql
SELECT * FROM points
ORDER BY x
LIMIT 3;

SELECT * FROM points
ORDER BY x
LIMIT 3 OFFSET 2;
```

---

## ✏️ UPDATE — изменение строк

| 📌 Команда                                          | 📖 Что делает              | ⚠️ Важно                          |
| --------------------------------------------------- | -------------------------- | --------------------------------- |
| `UPDATE points SET x = y * 2 + x;`                  | Меняет `x` во всех строках | Без `WHERE` меняет всё            |
| `UPDATE points SET x = y, y = x;`                   | Меняет `x` и `y` местами   | Значения берутся из старой строки |
| `UPDATE points SET x = 0 WHERE x < 0 OR x IS NULL;` | Исправляет `x`             | Работает только по условию        |
| `UPDATE points SET y = 0 WHERE y < 0 OR y IS NULL;` | Исправляет `y`             | Работает только по условию        |

```sql
UPDATE points
SET x = y * 2 + x;

SELECT * FROM points;

UPDATE points
SET x = y, y = x;

SELECT * FROM points;

UPDATE points
SET x = 0
WHERE x < 0 OR x IS NULL;

UPDATE points
SET y = 0
WHERE y < 0 OR y IS NULL;

SELECT * FROM points;
```

---

## 🗑 DROP TABLE

| 📌 Команда                                   | 📖 Что делает                               | ⚠️ Важно                                         |
| -------------------------------------------- | ------------------------------------------- | ------------------------------------------------ |
| `CREATE TABLE "My ugly table name"`          | Создаёт таблицу с именем в двойных кавычках | Двойные кавычки используются для идентификаторов |
| `DROP TABLE "My ugly table name";`           | Удаляет таблицу                             | Если повторить, будет ошибка                     |
| `DROP TABLE IF EXISTS "My ugly table name";` | Удаляет только если существует              | Безопасный вариант                               |
| `DROP TABLE points;`                         | Удаляет таблицу `points`                    | Таблица исчезает полностью                       |

```sql
CREATE TABLE "My ugly table name" (
    aaa integer,
    "b b b" text
);

DROP TABLE IF EXISTS "My ugly table name";

DROP TABLE points;
```

---

## 🧪 Полный практический сценарий

```sql
DROP DATABASE IF EXISTS yandex_test;
CREATE DATABASE yandex_test;
\c yandex_test

SELECT current_database();

SELECT 2 + 2;
SELECT 10 * 5 AS result;
SELECT length('Hello, Practicum');
SELECT 'User''s name';
SELECT E'Hello, \'Postgres\'!\nHello, user!';

CREATE TABLE points (
    x integer,
    y integer
);

\dt
\d points

INSERT INTO points VALUES (10, -20);
INSERT INTO points VALUES (0, 0), (1, 1), (2, 2);
INSERT INTO points (y) VALUES (50);
INSERT INTO points VALUES (100, DEFAULT);

SELECT * FROM points;

SELECT x, y, x + y AS sum FROM points;
SELECT x, y, @x + @y AS abs_sum FROM points;
SELECT x, y, |/(x*x + y*y) AS dist_to_0 FROM points;

DELETE FROM points
WHERE x IS NULL OR y IS NULL;

SELECT * FROM points
WHERE x > 0 AND y > 0;

DELETE FROM points;

INSERT INTO points VALUES
    (10, -20),
    (0, 0),
    (1, 1),
    (1, 2),
    (DEFAULT, 50),
    (100, DEFAULT);

SELECT * FROM points ORDER BY x;
SELECT * FROM points ORDER BY x DESC;
SELECT * FROM points ORDER BY x ASC;
SELECT * FROM points ORDER BY x DESC, y DESC;

SELECT * FROM points
ORDER BY x
LIMIT 3;

SELECT * FROM points
ORDER BY x
LIMIT 3 OFFSET 2;

UPDATE points
SET x = y * 2 + x;

SELECT * FROM points;

UPDATE points
SET x = y, y = x;

SELECT * FROM points;

UPDATE points
SET x = 0
WHERE x < 0 OR x IS NULL;

UPDATE points
SET y = 0
WHERE y < 0 OR y IS NULL;

SELECT * FROM points;

CREATE TABLE "My ugly table name" (
    aaa integer,
    "b b b" text
);

DROP TABLE IF EXISTS "My ugly table name";

DROP TABLE points;

\dt

\c postgres

DROP DATABASE yandex_test;
```

---

## ✅ Главное запомнить

| 📌 Команда        | 📖 Назначение    | ⚠️ Важно                              |
| ----------------- | ---------------- | ------------------------------------- |
| `CREATE DATABASE` | Создать базу     | Создаёт отдельное пространство данных |
| `DROP DATABASE`   | Удалить базу     | Удаляет всё внутри базы               |
| `CREATE TABLE`    | Создать таблицу  | Описывает столбцы                     |
| `INSERT INTO`     | Добавить строки  | Можно добавлять одну или несколько    |
| `SELECT`          | Прочитать данные | Основная команда чтения               |
| `WHERE`           | Условие          | Фильтрует строки                      |
| `DELETE FROM`     | Удалить строки   | Без `WHERE` удаляет все строки        |
| `ORDER BY`        | Сортировка       | `ASC` или `DESC`                      |
| `LIMIT`           | Ограничение      | Сколько строк вернуть                 |
| `OFFSET`          | Смещение         | Сколько строк пропустить              |
| `UPDATE`          | Изменить строки  | Без `WHERE` меняет все строки         |
| `DROP TABLE`      | Удалить таблицу  | Удаляет структуру и данные            |

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../README.md)

[Вернуться в Sprint 4](../../../README.md)
