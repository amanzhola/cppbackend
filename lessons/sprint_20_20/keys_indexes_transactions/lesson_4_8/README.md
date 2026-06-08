# 🔑 Lesson 4/8 — Ключи и индексы PostgreSQL

## 📌 Описание урока

| 📌 Блок         | 📖 Что изучается                                                                           | 🧠 Практическая ценность                                          | ✅ Результат                           |
| --------------- | ------------------------------------------------------------------------------------------ | ----------------------------------------------------------------- | ------------------------------------- |
| Ключи           | `PRIMARY KEY`, `FOREIGN KEY`, `REFERENCES`, `UUID`, `SERIAL`                               | Гарантируют уникальность строк и корректные связи между таблицами | Освоены первичные и внешние ключи     |
| Индексы         | `CREATE INDEX`, `UNIQUE INDEX`, составные индексы, частичные индексы, индексы по выражению | Ускоряют поиск, сортировку и фильтрацию данных                    | Освоена базовая оптимизация запросов  |
| Анализ запросов | `\timing on`, `EXPLAIN`, `Seq Scan`, `Index Scan`, `Sort`                                  | Позволяет увидеть, как PostgreSQL выполняет запрос                | Освоен анализ плана выполнения        |
| Агрегация       | `count(*)`, `GROUP BY`, `ORDER BY`, `LIMIT`, `OFFSET`                                      | Помогает анализировать большие наборы данных                      | Освоены базовые аналитические запросы |

---

## 📂 Структура урока

| 📄 Файл                                     | 📖 Назначение                                                      | ▶️ Запуск                                                                                         |
| ------------------------------------------- | ------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------- |
| `README.md`                                 | Конспект урока и описание практик                                  | Читать как инструкцию                                                                             |
| `practice_01_primary_foreign_keys.sql`      | Практика `PRIMARY KEY`, `FOREIGN KEY`, `UUID`, `SERIAL`            | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_primary_foreign_keys.sql`      |
| `practice_02_movies_import_notes.sql`       | Подготовка таблицы `movies` для большого импорта                   | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_movies_import_notes.sql`       |
| `practice_03_count_and_aggregation.sql`     | `count`, `GROUP BY`, агрегация и запрос по `director_id = 42`      | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_count_and_aggregation.sql`     |
| `practice_04_timing_and_director_index.sql` | Измерение времени и индекс по `director_id`                        | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_04_timing_and_director_index.sql` |
| `practice_05_title_index_and_explain.sql`   | Индекс по `title` и анализ через `EXPLAIN`                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_05_title_index_and_explain.sql`   |
| `practice_06_multi_index.sql`               | Составной индекс `(release_year, title)`                           | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_06_multi_index.sql`               |
| `practice_07_unique_index.sql`              | Уникальный индекс `(director_id, title)`                           | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_07_unique_index.sql`              |
| `practice_08_expression_partial_index.sql`  | Индекс по выражению `(sum_votes / votes_count)` и частичный индекс | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_08_expression_partial_index.sql`  |

---

## 🧠 Главная идея

| 📌 Идея                   | 📖 Объяснение                                                    |
| ------------------------- | ---------------------------------------------------------------- |
| `WHERE` ищет строки       | Чем больше таблица, тем дороже полный поиск                      |
| Индекс ускоряет `WHERE`   | PostgreSQL может найти строки быстрее, не читая всю таблицу      |
| Ключи защищают данные     | `PRIMARY KEY` и `FOREIGN KEY` не дают создать некорректные связи |
| `EXPLAIN` показывает план | Можно увидеть `Seq Scan`, `Index Scan`, `Sort`, `Filter`         |
| Индекс не всегда нужен    | Индексы ускоряют чтение, но замедляют вставку и занимают место   |

---

## 🔤 Расшифровки

| Термин      | Расшифровка                   | Значение                                |
| ----------- | ----------------------------- | --------------------------------------- |
| SQL         | Structured Query Language     | Язык структурированных запросов         |
| PRIMARY KEY | первичный ключ                | Главный уникальный идентификатор строки |
| FOREIGN KEY | внешний ключ                  | Ссылка на строку другой таблицы         |
| UUID        | Universally Unique Identifier | Универсально уникальный идентификатор   |
| SERIAL      | integer + sequence            | Автоувеличивающийся идентификатор       |
| INDEX       | индекс                        | Структура для ускорения поиска          |
| idx         | index                         | Частое сокращение в названии индекса    |
| EXPLAIN     | объяснить                     | Показывает план выполнения запроса      |

---

## 🧪 Практика 1 — PRIMARY KEY и FOREIGN KEY

| 📌 Шаг | SQL / команда                                 | 📖 Что делает                                                         |
| ------ | --------------------------------------------- | --------------------------------------------------------------------- |
| 1      | `DROP TABLE IF EXISTS films;`                 | Удаляет таблицу фильмов первой, потому что она ссылается на `persons` |
| 2      | `DROP TABLE IF EXISTS persons;`               | Удаляет таблицу персон                                                |
| 3      | `CREATE EXTENSION IF NOT EXISTS pgcrypto;`    | Подключает `gen_random_uuid()`                                        |
| 4      | `CREATE TABLE persons (...)`                  | Создаёт таблицу персон с UUID                                         |
| 5      | `CREATE TABLE films (...)`                    | Создаёт таблицу фильмов с внешним ключом                              |
| 6      | `INSERT INTO persons ...`                     | Добавляет Christopher Nolan                                           |
| 7      | `INSERT INTO films ... SELECT ...`            | Добавляет фильм, связанный с существующим режиссёром                  |
| 8      | `SELECT * FROM persons; SELECT * FROM films;` | Проверяет результат                                                   |

Файл `practice_01_primary_foreign_keys.sql`:

```sql
DROP TABLE IF EXISTS films;
DROP TABLE IF EXISTS persons;

CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TABLE persons (
    first_name text,
    last_name text,
    id uuid PRIMARY KEY DEFAULT gen_random_uuid()
);

CREATE TABLE films (
    title text NOT NULL,
    director uuid REFERENCES persons(id) NOT NULL,
    id SERIAL PRIMARY KEY
);

INSERT INTO persons (first_name, last_name)
VALUES ('Christopher', 'Nolan');

INSERT INTO films (title, director)
SELECT 'Inception', id
FROM persons
WHERE last_name = 'Nolan';

SELECT * FROM persons;
SELECT * FROM films;
```

Проверка ошибки внешнего ключа вручную:

```sql
INSERT INTO films (title, director)
VALUES ('Fake film', '6d4abf14-7a60-11ed-a1eb-0242ac120002');
```

Ожидаемый смысл:

```text
violates foreign key constraint
```

---

## 🎬 Практика 2 — таблица movies для большого импорта

| Поле           | Тип                  | Назначение                      |
| -------------- | -------------------- | ------------------------------- |
| `id`           | `SERIAL PRIMARY KEY` | Уникальный идентификатор фильма |
| `director_id`  | `int`                | Идентификатор режиссёра         |
| `release_year` | `integer`            | Год выпуска                     |
| `title`        | `text`               | Название фильма                 |
| `votes_count`  | `integer DEFAULT(0)` | Количество голосов              |
| `sum_votes`    | `float4 DEFAULT(0)`  | Сумма голосов                   |

Файл `practice_02_movies_import_notes.sql`:

```sql
DROP TABLE IF EXISTS movies;

CREATE TABLE movies (
    id SERIAL PRIMARY KEY,
    director_id int,
    release_year integer,
    title text,
    votes_count integer DEFAULT(0),
    sum_votes float4 DEFAULT(0)
);
```

Импорт `movies.sql` после копирования в папку урока:

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-d postgres \
-f ~/cppbackend/lessons/sprint_20_20/keys_indexes_transactions/lesson_4_8/movies.sql
```

Защита от деления на ноль при рейтинге:

```sql
sum_votes / NULLIF(votes_count, 0)
```

---

## 📊 Практика 3 — count и агрегация

| Запрос                                   | Что показывает                            |
| ---------------------------------------- | ----------------------------------------- |
| `SELECT count(*) FROM movies;`           | Общее количество строк                    |
| `SELECT count(title) FROM movies;`       | Количество строк с непустым `title`       |
| `SELECT count(director_id) FROM movies;` | Количество строк с непустым `director_id` |
| `GROUP BY release_year`                  | Группировка фильмов по году               |
| `WHERE director_id = 42`                 | Количество фильмов конкретного режиссёра  |

Файл `practice_03_count_and_aggregation.sql`:

```sql
SELECT count(*) FROM movies;

SELECT count(title) FROM movies;

SELECT count(director_id) FROM movies;

SELECT release_year, count(*)
FROM movies
GROUP BY release_year
ORDER BY release_year DESC
LIMIT 10;

SELECT count(*)
FROM movies
WHERE director_id = 42;
```

Ожидаемый ориентир после импорта:

```text
SELECT count(*) FROM movies; -> 1000000
```

---

## ⏱ Практика 4 — timing и индекс по director_id

| Шаг | SQL                                                         | Назначение                         |
| --- | ----------------------------------------------------------- | ---------------------------------- |
| 1   | `\timing on`                                                | Включить измерение времени         |
| 2   | `SELECT count(*) FROM movies WHERE director_id = 42;`       | Проверить скорость до индекса      |
| 3   | `CREATE INDEX movies_director_idx ON movies (director_id);` | Создать индекс                     |
| 4   | Повторить `SELECT`                                          | Проверить ускорение                |
| 5   | `\d movies`                                                 | Увидеть индекс в структуре таблицы |

Файл `practice_04_timing_and_director_index.sql`:

```sql
\timing on

SELECT count(*) FROM movies WHERE director_id = 42;

CREATE INDEX movies_director_idx ON movies (director_id);

SELECT count(*) FROM movies WHERE director_id = 42;

\d movies
```

---

## 🧭 Практика 5 — индекс по title и EXPLAIN

| Запрос                                             | Что изучается                           |
| -------------------------------------------------- | --------------------------------------- |
| `ORDER BY title LIMIT 10`                          | Сортировка по названию                  |
| `CREATE INDEX movies_title_idx ON movies (title);` | Индекс по названию                      |
| `EXPLAIN ... ORDER BY title`                       | План выполнения с индексом              |
| `EXPLAIN ... ORDER BY votes_count`                 | План выполнения без подходящего индекса |

Файл `practice_05_title_index_and_explain.sql`:

```sql
\timing on

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

CREATE INDEX movies_title_idx ON movies (title);

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY votes_count
LIMIT 10;
```

Слова в плане:

| Термин         | Значение                                         |
| -------------- | ------------------------------------------------ |
| `Seq Scan`     | Sequential Scan, последовательное чтение таблицы |
| `Index Scan`   | Чтение через индекс                              |
| `Filter`       | Фильтрация по `WHERE`                            |
| `Sort`         | Сортировка                                       |
| `Gather Merge` | Сбор результатов параллельной обработки          |

---

## 🧱 Практика 6 — составной индекс

| Элемент                 | Значение                                       |
| ----------------------- | ---------------------------------------------- |
| `LIMIT 10 OFFSET 1000`  | Пропустить 1000 строк и взять следующие 10     |
| `(release_year, title)` | Составной индекс по году и названию            |
| Польза                  | Быстрый поиск по году и сортировка по названию |

Файл `practice_06_multi_index.sql`:

```sql
\timing on

DROP INDEX IF EXISTS movies_title_idx;

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;

CREATE INDEX movies_year_title_idx ON movies (release_year, title);

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;
```

---

## 🧬 Практика 7 — UNIQUE индекс

| Элемент                 | Значение                                  |
| ----------------------- | ----------------------------------------- |
| `CREATE UNIQUE INDEX`   | Создаёт уникальный индекс                 |
| `(director_id, title)`  | Запрещает повтор пары режиссёр + название |
| Повторная вставка дубля | Должна привести к ошибке                  |

Файл `practice_07_unique_index.sql`:

```sql
CREATE UNIQUE INDEX movies_director_title_idx
ON movies (director_id, title);

SELECT *
FROM movies
WHERE director_id = 42
LIMIT 1;
```

Проверка вручную:

```sql
INSERT INTO movies (director_id, release_year, title)
VALUES (42, 1902, 'PASTE_TITLE_HERE');
```

---

## 🧮 Практика 8 — индекс по выражению и частичный индекс

| Элемент                     | Значение                                   |
| --------------------------- | ------------------------------------------ |
| `(sum_votes / votes_count)` | Индекс по вычисляемому выражению           |
| `WHERE votes_count > 0`     | Частичный индекс                           |
| `votes_count >= 10`         | Запрос учитывает только фильмы с голосами  |
| Цель                        | Быстрее получать рейтинг фильмов режиссёра |

Файл `practice_08_expression_partial_index.sql`:

```sql
\timing on

SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;

SELECT count(*)
FROM movies
WHERE votes_count = 0;

CREATE INDEX movies_director_rating_idx
ON movies (director_id, (sum_votes / votes_count))
WHERE votes_count > 0;

SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;

EXPLAIN
SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;
```

---

## ✅ Главное запомнить

| Тема                     | Правило                                      |
| ------------------------ | -------------------------------------------- |
| `PRIMARY KEY`            | Уникально идентифицирует строку              |
| `FOREIGN KEY`            | Гарантирует ссылку на существующую строку    |
| `UUID`                   | Можно генерировать через `gen_random_uuid()` |
| `SERIAL`                 | Удобен для автоувеличивающегося `integer`    |
| `CREATE INDEX`           | Ускоряет запросы по `WHERE` и `ORDER BY`     |
| `UNIQUE INDEX`           | Не только ускоряет, но и запрещает дубли     |
| Составной индекс         | Важен порядок полей                          |
| Частичный индекс         | Индексирует только часть строк               |
| Индекс по выражению      | Индексирует вычисляемое значение             |
| `EXPLAIN`                | Показывает план выполнения                   |
| `\timing on`             | Показывает время выполнения запроса          |
| `NULLIF(votes_count, 0)` | Защищает от деления на ноль                  |

---

## 🏁 Итог

| 📌 Что изучено       | ✅ Статус |
| -------------------- | -------- |
| PRIMARY KEY          | ✅        |
| FOREIGN KEY          | ✅        |
| UUID                 | ✅        |
| SERIAL               | ✅        |
| pgcrypto             | ✅        |
| gen_random_uuid()    | ✅        |
| count и GROUP BY     | ✅        |
| timing               | ✅        |
| EXPLAIN              | ✅        |
| обычные индексы      | ✅        |
| составные индексы    | ✅        |
| уникальные индексы   | ✅        |
| частичные индексы    | ✅        |
| индексы по выражению | ✅        |

---

## ⬅️ Назад

[Вернуться к Keys, Indexes and Transactions](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
