# 📊 Lesson 5/8 — Агрегирование и вложенные запросы PostgreSQL

## 📌 Описание урока

| 📌 Блок             | 📖 Что изучается                                                     | 🧠 Практическая ценность                            | ✅ Результат                          |
| ------------------- | -------------------------------------------------------------------- | --------------------------------------------------- | ------------------------------------ |
| Агрегирование       | `sum`, `count`, `avg`, `min`, `max`                                  | Позволяет получать итоговые значения по таблице     | Освоены основные агрегатные функции  |
| Группировка         | `GROUP BY`, `HAVING`, группировка по `year`, `genre`, `country`      | Позволяет считать итоги отдельно для каждой группы  | Освоена аналитика по группам         |
| Вложенные запросы   | `SELECT ... FROM (...) AS alias`, подзапросы в `HAVING`              | Позволяет решать задачи в несколько этапов          | Освоен поиск максимумов по агрегатам |
| Анализ запроса      | `EXPLAIN`, `HashAggregate`, `Seq Scan`, `Filter`, `InitPlan`         | Показывает, как PostgreSQL выполняет сложный запрос | Освоено чтение базового плана        |
| Практические задачи | Жанры, страны, прибыль, отношение бюджета к сборам, длинные названия | Закрепляет SQL на реальных аналитических вопросах   | Подготовлены 27 SQL-практик          |

---

## 📂 Структура урока

| 📄 Файл                                            | 📖 Назначение                                                            | ▶️ Запуск                                                                                                |
| -------------------------------------------------- | ------------------------------------------------------------------------ | -------------------------------------------------------------------------------------------------------- |
| `README.md`                                        | Конспект урока и описание практик                                        | Читать как инструкцию                                                                                    |
| `movies.sql`                                       | Импорт большой таблицы фильмов                                           | `psql -h localhost -p 30432 -U postgres -d postgres -f movies.sql`                                       |
| `practice_01_sum.sql`                              | Первые агрегаты: `sum`, `avg`, `min`, `max`, `count`                     | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_sum.sql`                              |
| `practice_02_sum.sql`                              | `sum(box)` и `count(*)`                                                  | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_sum.sql`                              |
| `practice_03_count.sql`                            | `count(*)`, фильтры по году и стране, `count(budget)`                    | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_count.sql`                            |
| `practice_04_count_column.sql`                     | Разница `count(*)` и `count(column)`, средний бюджет                     | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_04_count_column.sql`                     |
| `practice_05_avg.sql`                              | `avg(budget)`, средний бюджет Аргентины, максимальный бюджет             | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_05_avg.sql`                              |
| `practice_06_max.sql`                              | `max(budget)`, `max(box)`, `min(year)`                                   | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_06_max.sql`                              |
| `practice_07_min.sql`                              | Минимальный год и несколько агрегатов одним запросом                     | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_07_min.sql`                              |
| `practice_08_many_aggregates.sql`                  | `count`, `min`, `max`, `avg`, `sum` одной строкой                        | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_08_many_aggregates.sql`                  |
| `practice_09_ind_comedy_ratio.sql`                 | Отношение суммарного бюджета к суммарной кассе индийских комедий         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_09_ind_comedy_ratio.sql`                 |
| `practice_10_avg_budget_box_ratio.sql`             | Среднее `budget / box` для индийских комедий через целочисленное деление | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_10_avg_budget_box_ratio.sql`             |
| `practice_11_create_movies_temp.sql`               | Временная таблица `movies_temp`                                          | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_11_create_movies_temp.sql`               |
| `practice_11_15_group_by_full.sql`                 | Единый запуск практик `GROUP BY`, `HAVING`, `movies_temp`                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_11_15_group_by_full.sql`                 |
| `practice_12_group_by_year_genre.sql`              | Группировка по `year`, `genre`                                           | Запускать в той же `psql`-сессии, где есть `movies_temp`                                                 |
| `practice_13_group_by_filter_check.sql`            | Проверка строк конкретной группы                                         | Запускать в той же `psql`-сессии, где есть `movies_temp`                                                 |
| `practice_14_group_by_sum_box.sql`                 | Сумма кассы по группам `year`, `genre`                                   | Запускать в той же `psql`-сессии, где есть `movies_temp`                                                 |
| `practice_15_having.sql`                           | Фильтрация групп через `HAVING count(*) > 1`                             | Запускать в той же `psql`-сессии, где есть `movies_temp`                                                 |
| `practice_16_where_vs_having.sql`                  | Разница `WHERE` и `HAVING`                                               | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_16_where_vs_having.sql`                  |
| `practice_17_max_year_box.sql`                     | Максимальная годовая выручка через подзапрос                             | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_17_max_year_box.sql`                     |
| `practice_18_best_year.sql`                        | Поиск года с максимальной выручкой                                       | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_18_best_year.sql`                        |
| `practice_19_best_year_with_sum.sql`               | Год и сумма максимальной выручки                                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_19_best_year_with_sum.sql`               |
| `practice_20_explain_nested_query.sql`             | `EXPLAIN` для вложенного запроса                                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_20_explain_nested_query.sql`             |
| `practice_21_most_popular_genre.sql`               | Самый частый жанр через `ORDER BY count DESC LIMIT 1`                    | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_21_most_popular_genre.sql`               |
| `practice_22_most_popular_genre_with_subquery.sql` | Все жанры с максимальным количеством фильмов                             | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_22_most_popular_genre_with_subquery.sql` |
| `practice_23_most_popular_russian_genre.sql`       | Самый частый жанр среди российских фильмов                               | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_23_most_popular_russian_genre.sql`       |
| `practice_24_worst_box_budget_ratio.sql`           | Худшее отношение кассы к бюджету                                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_24_worst_box_budget_ratio.sql`           |
| `practice_25_longest_movie_title.sql`              | Самое длинное название фильма                                            | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_25_longest_movie_title.sql`              |
| `practice_26_best_profit_country.sql`              | Самая прибыльная страна по `sum(box - budget)`                           | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_26_best_profit_country.sql`              |
| `practice_27_countries_with_20_movies.sql`         | Количество стран с минимум 20 фильмами после 2000 года                   | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_27_countries_with_20_movies.sql`         |

---

## 🎬 Подготовка `movies.sql`

| 📌 Шаг | Команда                                                                     | 📖 Что делает            | ✅ Проверка                                            |
| ------ | --------------------------------------------------------------------------- | ------------------------ | ----------------------------------------------------- |
| 1      | `cd ~/cppbackend/lessons/sprint_20_20/keys_indexes_transactions/lesson_5_8` | Перейти в папку урока    | `pwd`                                                 |
| 2      | `cp ".../movies_new.sql" ./movies.sql`                                      | Скопировать файл импорта | `ls -lh`                                              |
| 3      | `DROP TABLE IF EXISTS movies CASCADE;`                                      | Удалить старую таблицу   | Таблица очищена                                       |
| 4      | `psql ... -f movies.sql`                                                    | Импортировать данные     | Таблица создана                                       |
| 5      | `psql ... -c "\d movies"`                                                   | Проверить структуру      | Есть поля `budget`, `year`, `country`, `genre`, `box` |

```bash
cd ~/cppbackend/lessons/sprint_20_20/keys_indexes_transactions/lesson_5_8

cp "/mnt/c/NFactorial/Dev c++/sprint1(plus)/sprint20/3. Ключи, индексы, транзакции/5. Агрегирование и вложенные запросы/movies_new.sql" ./movies.sql

ls -lh

psql -h localhost -p 30432 -U postgres -d postgres -c "DROP TABLE IF EXISTS movies CASCADE;"

psql -h localhost -p 30432 -U postgres -d postgres -f movies.sql

psql -h localhost -p 30432 -U postgres -d postgres -c "\d movies"
```

---

## 🧠 Агрегирование

| Термин          | Расшифровка                      | Смысл                                           | Пример                              |
| --------------- | -------------------------------- | ----------------------------------------------- | ----------------------------------- |
| `Aggregate`     | собрать, объединить, суммировать | Схлопывает много строк в одно итоговое значение | `sum(budget)`                       |
| `sum`           | сумма                            | Складывает значения столбца                     | `SELECT sum(box) FROM movies;`      |
| `count(*)`      | количество строк                 | Считает все строки                              | `SELECT count(*) FROM movies;`      |
| `count(column)` | количество непустых значений     | Не считает `NULL`                               | `SELECT count(budget) FROM movies;` |
| `avg`           | average, среднее                 | Сумма / количество                              | `SELECT avg(budget) FROM movies;`   |
| `min`           | минимум                          | Самое маленькое значение                        | `SELECT min(year) FROM movies;`     |
| `max`           | максимум                         | Самое большое значение                          | `SELECT max(box) FROM movies;`      |

---

## 🧪 Практики 1–8 — базовые агрегаты

| Практика                          | SQL                                                             | Что считает                            | Важное отличие                               |
| --------------------------------- | --------------------------------------------------------------- | -------------------------------------- | -------------------------------------------- |
| `practice_01_sum.sql`             | `sum`, `avg`, `min`, `max`, `count`                             | Первые агрегаты по таблице             | SQL выполняется через `psql`, не через bash  |
| `practice_02_sum.sql`             | `sum(box)`, `count(*)`                                          | Общая касса и количество фильмов       | `SUM` считает деньги, `COUNT` считает строки |
| `practice_03_count.sql`           | `count(*) WHERE year = 2011`, `country = 'Arg'`                 | Количество фильмов по фильтрам         | `WHERE` фильтрует строки до подсчёта         |
| `practice_04_count_column.sql`    | `count(budget)`, `avg(budget)`                                  | Непустые бюджеты и среднее             | `count(column)` не считает `NULL`            |
| `practice_05_avg.sql`             | `avg(budget)`, `avg(budget) WHERE country='Arg'`, `max(budget)` | Средний и максимальный бюджет          | Фильтр меняет набор строк                    |
| `practice_06_max.sql`             | `max(budget)`, `max(box)`, `min(year)`                          | Максимумы и самый старый год           | `MAX` и `MIN` ищут крайние значения          |
| `practice_07_min.sql`             | `min(year)` и несколько агрегатов                               | Один запрос может считать много итогов | Результат — одна строка                      |
| `practice_08_many_aggregates.sql` | `count`, `min`, `max`, `avg`, `sum`                             | Полная сводка по таблице               | Несколько агрегатов в одном SELECT           |

```sql
SELECT
    count(*),
    min(year),
    max(year),
    avg(budget),
    sum(box)
FROM movies;
```

---

## ⚠️ Bash и psql

| Где выполняется | Что писать                                                                                | Что нельзя писать                                      |
| --------------- | ----------------------------------------------------------------------------------------- | ------------------------------------------------------ |
| `bash`          | `psql -h localhost -p 30432 -U postgres -d postgres -c "SELECT sum(budget) FROM movies;"` | Нельзя писать просто `SELECT sum(budget) FROM movies;` |
| `psql`          | `SELECT sum(budget) FROM movies;`                                                         | Не нужно писать Linux-команды                          |
| Файл `.sql`     | SQL-команды                                                                               | Запускать через `psql ... -f file.sql`                 |

```bash
psql -h localhost -p 30432 -U postgres -d postgres -c "SELECT sum(budget) FROM movies;"
```

---

## 🧮 Практики 9–10 — отношения бюджета и кассы

| Практика                               | Запрос                            | Что считает                                                      | Важная ошибка / исправление                    |
| -------------------------------------- | --------------------------------- | ---------------------------------------------------------------- | ---------------------------------------------- |
| `practice_09_ind_comedy_ratio.sql`     | `sum(budget)::numeric / sum(box)` | Отношение суммарного бюджета к суммарной кассе индийских комедий | Здесь используется вещественное отношение сумм |
| `practice_10_avg_budget_box_ratio.sql` | `avg(budget / box)`               | Среднее целочисленных отношений `budget / box`                   | Для задания важно не приводить к `real`        |

```sql
SELECT sum(budget)::numeric / sum(box) AS ratio
FROM movies
WHERE country = 'Ind'
  AND genre = 'comedy';
```

```sql
SELECT avg(budget / box)
FROM movies
WHERE country = 'Ind'
  AND genre = 'comedy';
```

Ожидаемый вариант для `practice_10`:

```text
0.029
```

Причина:

| Выражение            | Тип деления                      | Пример                 | Итог               |
| -------------------- | -------------------------------- | ---------------------- | ------------------ |
| `budget / box`       | `bigint / bigint`, целочисленное | `100 / 300 = 0`        | Даёт около `0.029` |
| `budget::real / box` | вещественное                     | `100 / 300 = 0.333...` | Даёт другой ответ  |

---

## 🧱 GROUP BY

| Конструкция              | Значение                         | Пример                         |
| ------------------------ | -------------------------------- | ------------------------------ |
| `GROUP BY`               | сгруппировать строки             | `GROUP BY year, genre`         |
| `count(*)` внутри группы | количество строк в каждой группе | `SELECT year, genre, count(*)` |
| `sum(box)` внутри группы | сумма кассы внутри группы        | `SELECT year, genre, sum(box)` |
| `HAVING`                 | фильтр групп после группировки   | `HAVING count(*) > 1`          |
| `WHERE`                  | фильтр строк до группировки      | `WHERE year >= 1990`           |

---

## 🚫 Почему нельзя `SELECT * FROM movies_temp GROUP BY year, genre`

| Что просим             | Почему проблема                                                       |
| ---------------------- | --------------------------------------------------------------------- |
| `GROUP BY year, genre` | Каждая итоговая строка — одна группа `year + genre`                   |
| `SELECT *`             | Просит вывести `id`, `name`, `country`, `budget`, `box` и другие поля |
| Внутри группы          | Может быть много разных `id`, `name`, `budget`, `box`                 |
| Итог                   | PostgreSQL не знает, какое значение показать                          |

Правильно:

```sql
SELECT year, genre, count(*)
FROM movies_temp
GROUP BY year, genre;
```

или:

```sql
SELECT
    year,
    genre,
    min(id) AS min_id,
    max(id) AS max_id,
    count(*) AS movies_count
FROM movies_temp
GROUP BY year, genre;
```

Правило:

```text
Все столбцы в SELECT должны быть либо в GROUP BY, либо внутри агрегатной функции.
```

---

## 🧪 Практики 11–15 — временная таблица, GROUP BY, HAVING

| Практика                                | SQL                                                              | Что делает                                     | Важное замечание                          |
| --------------------------------------- | ---------------------------------------------------------------- | ---------------------------------------------- | ----------------------------------------- |
| `practice_11_create_movies_temp.sql`    | `CREATE TEMPORARY TABLE movies_temp (LIKE movies INCLUDING ALL)` | Создаёт временную таблицу                      | Живёт только в текущей `psql`-сессии      |
| `practice_11_15_group_by_full.sql`      | Все запросы 11–15 одним файлом                                   | Надёжный способ запускать `movies_temp`        | Лучше для `psql ... -f`                   |
| `practice_12_group_by_year_genre.sql`   | `GROUP BY year, genre`                                           | Считает фильмы по году и жанру                 | Отдельно запускать только в той же сессии |
| `practice_13_group_by_filter_check.sql` | `WHERE year = 1994 AND genre = 'drama'`                          | Показывает строки конкретной группы            | Проверка результата группировки           |
| `practice_14_group_by_sum_box.sql`      | `sum(box) GROUP BY year, genre`                                  | Суммарная касса по группам                     | `total_box` = общий box office            |
| `practice_15_having.sql`                | `HAVING count(*) > 1`                                            | Оставляет только группы с несколькими фильмами | `HAVING` работает после `GROUP BY`        |

```sql
DROP TABLE IF EXISTS movies_temp;

CREATE TEMPORARY TABLE movies_temp
(
    LIKE movies INCLUDING ALL
);

INSERT INTO movies_temp
SELECT *
FROM movies
LIMIT 50;

SELECT year, genre, count(*) AS count
FROM movies_temp
GROUP BY year, genre;

SELECT year, genre, count(*) AS movies_count
FROM movies_temp
GROUP BY year, genre
HAVING count(*) > 1
ORDER BY movies_count DESC, year, genre;
```

---

## 🧭 WHERE и HAVING

| Конструкция | Когда применяется | Что фильтрует               | Пример                             |
| ----------- | ----------------- | --------------------------- | ---------------------------------- |
| `WHERE`     | До группировки    | Отдельные строки            | `WHERE year BETWEEN 2000 AND 2020` |
| `GROUP BY`  | После `WHERE`     | Создаёт группы              | `GROUP BY year`                    |
| `HAVING`    | После группировки | Группы                      | `HAVING sum(box) > 25000000000`    |
| `ORDER BY`  | После `SELECT`    | Итоговый результат          | `ORDER BY year`                    |
| `LIMIT`     | В самом конце     | Количество строк результата | `LIMIT 10`                         |

Логический порядок:

```text
FROM → WHERE → GROUP BY → HAVING → SELECT → ORDER BY → LIMIT
```

`practice_16_where_vs_having.sql`:

```sql
SELECT year,
       sum(box) AS total_box
FROM movies
WHERE year BETWEEN 2000 AND 2020
GROUP BY year
HAVING sum(box) > 25000000000
ORDER BY year;
```

---

## 🧩 Вложенные запросы

| Задача                             | Почему нужен подзапрос                              | Правильная идея                                                |
| ---------------------------------- | --------------------------------------------------- | -------------------------------------------------------------- |
| Найти максимальную годовую выручку | Нельзя писать `max(sum(box))` на одном уровне       | Сначала `sum(box) GROUP BY year`, потом внешний `max(box_sum)` |
| Найти год с максимальной выручкой  | Нужно сравнить сумму каждого года с максимумом      | `HAVING sum(box) = (SELECT max(...))`                          |
| Найти жанры-рекордсмены            | Может быть несколько жанров с одинаковым максимумом | `HAVING count(*) = (SELECT max(...))`                          |
| Посчитать страны с 20+ фильмами    | Сначала выбрать страны, потом посчитать их          | Внешний `count(*) FROM (...)`                                  |

Ошибка:

```sql
SELECT max(sum(box))
FROM movies
GROUP BY year;
```

Причина:

```text
aggregate function calls cannot be nested
```

Правильно:

```sql
SELECT max(box_sum)
FROM (
      SELECT sum(box) AS box_sum
      FROM movies
      GROUP BY year
     ) AS sum_tab;
```

---

## 🧪 Практики 17–19 — лучший год по кассе

| Практика                             | Что ищет                     | SQL-идея                             | Ожидаемый смысл |              |
| ------------------------------------ | ---------------------------- | ------------------------------------ | --------------- | ------------ |
| `practice_17_max_year_box.sql`       | Максимальную годовую выручку | `SELECT max(box_sum) FROM (...)`     | Одно число      |              |
| `practice_18_best_year.sql`          | Год с максимальной выручкой  | `HAVING sum(box) = (подзапрос max)`  | Например `2019` |              |
| `practice_19_best_year_with_sum.sql` | Год и сумму                  | `SELECT year, sum(box) AS total_box` | Например `2019  | 33042435000` |

```sql
SELECT year,
       sum(box) AS total_box
FROM movies
WHERE year BETWEEN 2000 AND 2020
GROUP BY year
HAVING sum(box)=(
    SELECT max(box_sum)
    FROM (
        SELECT sum(box) AS box_sum
        FROM movies
        WHERE year BETWEEN 2000 AND 2020
        GROUP BY year
    ) AS sum_tab
);
```

---

## 🧠 Разбор вариантов для лучшего года

| Вариант                                                 | Статус | Причина                                                                         |
| ------------------------------------------------------- | ------ | ------------------------------------------------------------------------------- |
| `SELECT year ... GROUP BY year HAVING sum(box) = (...)` | ✅      | Корректно сравнивает сумму года с максимумом                                    |
| `SELECT * ... GROUP BY year ORDER BY year HAVING ...`   | ❌      | `HAVING` должен быть до `ORDER BY`, а `SELECT *` невозможен при `GROUP BY year` |
| `WHERE ... AND sum(box) = (...) GROUP BY year`          | ❌      | `WHERE` не знает агрегат `sum(box)`                                             |
| `HAVING sum(box)`                                       | ❌      | `HAVING` ждёт логическое выражение `TRUE/FALSE`                                 |

---

## 🔍 EXPLAIN для вложенного запроса

| Термин          | Расшифровка         | Значение                           |
| --------------- | ------------------- | ---------------------------------- |
| `EXPLAIN`       | объяснить           | Показывает план выполнения запроса |
| `HashAggregate` | hash + aggregate    | Группировка через хеш-таблицу      |
| `Seq Scan`      | Sequential Scan     | Последовательное чтение таблицы    |
| `Filter`        | фильтр              | Условие `WHERE`                    |
| `InitPlan`      | initialization plan | Подзапрос, вычисляемый заранее     |
| `Group Key`     | ключ группировки    | Поля из `GROUP BY`                 |

`practice_20_explain_nested_query.sql`:

```sql
EXPLAIN
SELECT year
FROM movies
WHERE year BETWEEN 2000 AND 2020
GROUP BY year
HAVING sum(box) =
(
    SELECT max(box_sum)
    FROM
    (
        SELECT sum(box) AS box_sum
        FROM movies
        WHERE year BETWEEN 2000 AND 2020
        GROUP BY year
    ) AS sum_tab
);
```

---

## 🎭 Практики 21–23 — жанры

| Практика                                           | Вопрос                     | Запрос                                       | Особенность                  |
| -------------------------------------------------- | -------------------------- | -------------------------------------------- | ---------------------------- |
| `practice_21_most_popular_genre.sql`               | Какой жанр самый частый?   | `GROUP BY genre ORDER BY count DESC LIMIT 1` | Показывает одного победителя |
| `practice_22_most_popular_genre_with_subquery.sql` | Все жанры с максимумом     | `HAVING count(*) = (SELECT max(...))`        | Учитывает ничью              |
| `practice_23_most_popular_russian_genre.sql`       | Самый частый жанр в России | `WHERE country = 'Rus' GROUP BY genre`       | Фильтр до группировки        |

```sql
SELECT genre,
       count(*) AS movies_count
FROM movies
GROUP BY genre
ORDER BY movies_count DESC
LIMIT 1;
```

```sql
SELECT genre,
       count(*) AS movies_count
FROM movies
WHERE country = 'Rus'
GROUP BY genre
ORDER BY movies_count DESC
LIMIT 1;
```

---

## 💰 Практики 24–27 — аналитические задачи

| Практика                                   | Вопрос                                            | SQL-идея                                                   | Результат                 |
| ------------------------------------------ | ------------------------------------------------- | ---------------------------------------------------------- | ------------------------- |
| `practice_24_worst_box_budget_ratio.sql`   | У какого фильма худшее отношение кассы к бюджету? | `box::float / budget ORDER BY ASC LIMIT 1`                 | Фильм с минимальным ratio |
| `practice_25_longest_movie_title.sql`      | У какого фильма самое длинное название?           | `length(name) ORDER BY DESC LIMIT 1`                       | Название и длина          |
| `practice_26_best_profit_country.sql`      | Какая страна самая прибыльная?                    | `sum(box - budget) GROUP BY country ORDER BY DESC LIMIT 1` | Страна и прибыль          |
| `practice_27_countries_with_20_movies.sql` | Сколько стран сняли 20+ фильмов после 2000 года?  | Подзапрос со странами и внешний `count(*)`                 | Количество стран          |

```sql
SELECT name,
       box,
       budget,
       box::float / budget AS box_budget_ratio
FROM movies
WHERE box > 0
ORDER BY box_budget_ratio ASC
LIMIT 1;
```

```sql
SELECT name,
       length(name) AS title_length
FROM movies
ORDER BY title_length DESC
LIMIT 1;
```

```sql
SELECT country,
       sum(box - budget) AS total_profit
FROM movies
GROUP BY country
ORDER BY total_profit DESC
LIMIT 1;
```

```sql
SELECT count(*) AS countries_count
FROM (
    SELECT country
    FROM movies
    WHERE year >= 2000
    GROUP BY country
    HAVING count(*) >= 20
) AS countries;
```

---

## ✅ Главное запомнить

| Тема            | Правило                    | Пример                        |
| --------------- | -------------------------- | ----------------------------- |
| `SUM`           | Складывает значения        | `sum(box)`                    |
| `COUNT(*)`      | Считает строки             | `count(*)`                    |
| `COUNT(column)` | Считает не-NULL значения   | `count(budget)`               |
| `AVG`           | Считает среднее            | `avg(budget)`                 |
| `MIN` / `MAX`   | Ищут минимум и максимум    | `min(year)`, `max(box)`       |
| `GROUP BY`      | Создаёт группы             | `GROUP BY year, genre`        |
| `HAVING`        | Фильтрует группы           | `HAVING count(*) > 1`         |
| `WHERE`         | Фильтрует строки           | `WHERE country = 'Rus'`       |
| Подзапрос       | Даёт промежуточную таблицу | `FROM (...) AS alias`         |
| `EXPLAIN`       | Показывает план            | `EXPLAIN SELECT ...`          |
| `LIMIT 1`       | Берёт победителя           | `ORDER BY count DESC LIMIT 1` |
| `ORDER BY DESC` | Сортирует по убыванию      | `ORDER BY total_profit DESC`  |

---

## 🏁 Итог

| 📌 Что изучено                         | ✅ Статус |
| -------------------------------------- | -------- |
| Агрегатные функции SQL                 | ✅        |
| `sum`, `count`, `avg`, `min`, `max`    | ✅        |
| Разница `count(*)` и `count(column)`   | ✅        |
| `GROUP BY`                             | ✅        |
| `HAVING`                               | ✅        |
| Разница `WHERE` и `HAVING`             | ✅        |
| Временные таблицы                      | ✅        |
| Вложенные запросы                      | ✅        |
| Запрет `max(sum(...))` на одном уровне | ✅        |
| Поиск года с максимальной кассой       | ✅        |
| Поиск самого частого жанра             | ✅        |
| Практические аналитические SQL-задачи  | ✅        |
| `EXPLAIN` для подзапроса               | ✅        |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add lessons/sprint_20_20/keys_indexes_transactions/lesson_5_8

git commit -m "Add aggregation and nested query practices"

git push
```

---

## ⬅️ Назад

[Вернуться к Keys, Indexes and Transactions](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
