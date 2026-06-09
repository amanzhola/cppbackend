# 🔗 Lesson 6/8 — Запросы к нескольким таблицам PostgreSQL

## 📌 Описание урока

| 📌 Блок              | 📖 Что изучается                                                                      | 🧠 Практическая ценность                                   | ✅ Результат                        |
| -------------------- | ------------------------------------------------------------------------------------- | ---------------------------------------------------------- | ---------------------------------- |
| JOIN                 | `JOIN`, `INNER JOIN`, `LEFT JOIN`, `ON`, декартово произведение                       | Позволяет получать связанные данные из нескольких таблиц   | Освоены базовые соединения         |
| Связанные таблицы    | `movies`, `awards`, `persons`, `relations`                                            | Позволяет работать с фильмами, наградами, людьми и ролями  | Освоена структура связей           |
| Фильтрация           | `WHERE`, `LIKE`, шаблон `%`, фильтр по стране, роли, награде                          | Позволяет выбирать только нужные строки после соединения   | Освоена фильтрация JOIN-результата |
| Агрегация после JOIN | `GROUP BY`, `count(*)`, `count(awards.id)`, `HAVING`                                  | Позволяет считать награды, фильмы, актёров и режиссёров    | Освоены аналитические JOIN-запросы |
| Исправления ошибок   | `count(*)` вместо `count(DISTINCT movies.id)`, вывод строк вместо внешнего `count(*)` | Позволяет получить именно тот формат, который ожидает курс | Исправлены практики 7 и 9          |

---

## 📂 Структура урока

| 📄 Файл                                              | 📖 Назначение                                                                       | ▶️ Запуск                                                                                                  |
| ---------------------------------------------------- | ----------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| `README.md`                                          | Конспект урока и описание практик                                                   | Читать как инструкцию                                                                                      |
| `practice_01_inner_join_movies_awards.sql`           | `INNER JOIN` таблиц `movies` и `awards` для финских фильмов                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_inner_join_movies_awards.sql`           |
| `practice_02_join_with_like.sql`                     | `JOIN` + `LIKE '%кот'` для наград, заканчивающихся на `кот`                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_join_with_like.sql`                     |
| `practice_03_left_join.sql`                          | `LEFT OUTER JOIN`, чтобы сохранить фильмы без наград                                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_left_join.sql`                          |
| `practice_04_three_tables_join.sql`                  | Соединение `relations`, `movies`, `persons` для актёров финских фильмов с 2016 года | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_04_three_tables_join.sql`                  |
| `practice_05_awards_count_by_genre_before_2000.sql`  | Количество наград по жанрам для фильмов до 2000 года                                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_05_awards_count_by_genre_before_2000.sql`  |
| `practice_06_movies_without_awards.sql`              | Подсчёт фильмов без наград через `LEFT JOIN` и `IS NULL`                            | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_06_movies_without_awards.sql`              |
| `practice_07_top_actor_by_movies_count.sql`          | Поиск актёра с максимальным количеством строк роли `актёр` в `relations`            | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_07_top_actor_by_movies_count.sql`          |
| `practice_08_movie_with_most_patricks.sql`           | Фильм с максимальным количеством наград, заканчивающихся на `Патрик`                | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_08_movie_with_most_patricks.sql`           |
| `practice_09_directors_with_more_than_one_oscar.sql` | Режиссёры, у которых больше одного `Оскара`                                         | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_09_directors_with_more_than_one_oscar.sql` |

---

## 🧠 Главная идея JOIN

| 📌 Термин                | 📖 Значение           | 🧠 Зачем нужен                                             |
| ------------------------ | --------------------- | ---------------------------------------------------------- |
| `JOIN`                   | соединить таблицы     | Получить данные, которые лежат в разных таблицах           |
| `ON`                     | условие соединения    | Указывает, какие строки из таблиц соответствуют друг другу |
| `INNER JOIN`             | внутреннее соединение | Показывает только строки, где есть совпадение              |
| `LEFT JOIN`              | левое соединение      | Показывает все строки левой таблицы, даже без совпадения   |
| `NULL` после `LEFT JOIN` | совпадение не найдено | Позволяет искать строки без связанных записей              |

Пример:

```sql
JOIN awards ON movies.id = awards.movie
```

Смысл:

```text
соединить фильм с его наградами
```

---

## 🧮 Декартово произведение и ON

| Ситуация                   | Что произойдёт                    | Почему опасно                                 |
| -------------------------- | --------------------------------- | --------------------------------------------- |
| Таблица `A` имеет 2 строки | `A1`, `A2`                        |                                               |
| Таблица `B` имеет 3 строки | `B1`, `B2`, `B3`                  |                                               |
| Соединение без условия     | Получится `2 * 3 = 6` строк       | Каждая строка A соединится с каждой строкой B |
| Соединение с `ON`          | Останутся только связанные строки | Результат становится осмысленным              |

```text
A1 B1
A1 B2
A1 B3
A2 B1
A2 B2
A2 B3
```

Поэтому почти всегда нужен:

```sql
ON movies.id = awards.movie
```

---

## 🎬 Таблицы урока

| Таблица     | Что хранит            | Важные поля                              | Как участвует в JOIN                    |
| ----------- | --------------------- | ---------------------------------------- | --------------------------------------- |
| `movies`    | Фильмы                | `id`, `name`, `country`, `year`, `genre` | Главная таблица фильмов                 |
| `awards`    | Награды               | `id`, `movie`, `award`                   | `awards.movie` ссылается на `movies.id` |
| `persons`   | Люди                  | `id`, `name`                             | Содержит имена актёров и режиссёров     |
| `relations` | Связи фильмов и людей | `movie`, `person`, `role`                | Связывает `movies` и `persons`          |

---

## 🧪 Практика 1 — INNER JOIN movies + awards

| SQL-часть                      | Что делает                     |
| ------------------------------ | ------------------------------ |
| `FROM movies`                  | Берёт фильмы                   |
| `JOIN awards`                  | Присоединяет награды           |
| `ON movies.id = awards.movie`  | Соединяет фильм с его наградой |
| `WHERE movies.country = 'Fin'` | Оставляет финские фильмы       |
| `ORDER BY movies.name`         | Сортирует по названию          |

```sql
SELECT *
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
ORDER BY movies.name;
```

Главное:

```text
Обычный JOIN = INNER JOIN.
Фильмы без наград не попадут в результат.
```

---

## 🧪 Практика 2 — JOIN + LIKE

| Элемент                    | Значение                                                       |
| -------------------------- | -------------------------------------------------------------- |
| `LIKE`                     | Соответствие шаблону                                           |
| `%`                        | Любой текст любой длины                                        |
| `'%кот'`                   | Любой текст, который заканчивается на `кот`                    |
| `awards.award LIKE '%кот'` | Награды вроде `Золотой кот`, `Серебряный кот`, `Бронзовый кот` |

```sql
SELECT *
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
  AND awards.award LIKE '%кот'
ORDER BY movies.name;
```

---

## 🧪 Практика 3 — LEFT OUTER JOIN

| JOIN                  | Что показывает              | Что скрывает                                 |
| --------------------- | --------------------------- | -------------------------------------------- |
| `JOIN` / `INNER JOIN` | Только фильмы с наградами   | Фильмы без наград                            |
| `LEFT JOIN`           | Все фильмы из левой таблицы | Ничего из левой таблицы не теряется          |
| `awards.* = NULL`     | Значит награды нет          | Используется для поиска отсутствующих связей |

```sql
SELECT *
FROM movies
LEFT OUTER JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
ORDER BY movies.name;
```

---

## 🧪 Практика 4 — JOIN трёх таблиц

| SQL-часть                                       | Что делает                  |
| ----------------------------------------------- | --------------------------- |
| `FROM relations`                                | Берёт таблицу связей        |
| `JOIN movies ON movies.id = relations.movie`    | Подтягивает данные фильма   |
| `JOIN persons ON persons.id = relations.person` | Подтягивает данные человека |
| `WHERE movies.country = 'Fin'`                  | Только финские фильмы       |
| `AND movies.year >= 2016`                       | Только фильмы с 2016 года   |
| `AND relations.role = 'актёр'`                  | Только актёры               |

```sql
SELECT *
FROM relations
JOIN movies ON movies.id = relations.movie
JOIN persons ON persons.id = relations.person
WHERE movies.country = 'Fin'
  AND movies.year >= 2016
  AND relations.role = 'актёр';
```

---

## ⚠️ Ambiguous column

| Проблема                             | Причина                                       | Решение                               |
| ------------------------------------ | --------------------------------------------- | ------------------------------------- |
| `column reference "id" is ambiguous` | В разных таблицах есть поле `id`              | Писать имя таблицы явно               |
| Плохо                                | `SELECT id`                                   | PostgreSQL не знает, какой `id` нужен |
| Хорошо                               | `SELECT movies.id`, `awards.id`, `persons.id` | Поле указано однозначно               |

`ambiguous`:

```text
двусмысленный / неоднозначный
```

---

## 🧪 Практика 5 — количество наград по жанрам до 2000 года

| SQL-часть                                      | Что делает                         |
| ---------------------------------------------- | ---------------------------------- |
| `SELECT movies.genre`                          | Выводит жанр                       |
| `count(awards.id) AS awards_count`             | Считает реальные награды           |
| `JOIN awards ON movies.id = awards.movie`      | Соединяет фильмы и награды         |
| `WHERE movies.year < 2000`                     | Только фильмы до 2000 года         |
| `GROUP BY movies.genre`                        | Группирует по жанру                |
| `ORDER BY awards_count DESC, movies.genre ASC` | Сортирует по количеству и алфавиту |

```sql
SELECT
    movies.genre,
    count(awards.id) AS awards_count
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.year < 2000
GROUP BY movies.genre
ORDER BY awards_count DESC, movies.genre ASC;
```

---

## 🔍 JOIN или LEFT JOIN для наград

| Вариант                            | Что считает                           | Когда использовать                     |
| ---------------------------------- | ------------------------------------- | -------------------------------------- |
| `JOIN awards`                      | Только фильмы, у которых есть награды | Когда нужны только награждённые фильмы |
| `LEFT JOIN awards`                 | Все фильмы, включая фильмы без наград | Когда нужно сохранить все фильмы       |
| `count(*)` при `LEFT JOIN`         | Считает даже фильм без награды        | Может дать неверный подсчёт наград     |
| `count(awards.id)` при `LEFT JOIN` | Считает только реальные награды       | Правильно для подсчёта наград          |

```sql
SELECT
    movies.genre,
    count(awards.id) AS awards_count
FROM movies
LEFT JOIN awards ON movies.id = awards.movie
WHERE movies.year < 2000
GROUP BY movies.genre
ORDER BY awards_count DESC, movies.genre ASC;
```

---

## 🧪 Практика 6 — фильмы без наград

| SQL-часть                                      | Что делает                                        |
| ---------------------------------------------- | ------------------------------------------------- |
| `FROM movies`                                  | Берёт все фильмы                                  |
| `LEFT JOIN awards ON movies.id = awards.movie` | Присоединяет награды, но сохраняет фильмы без них |
| `WHERE awards.id IS NULL`                      | Оставляет только фильмы, где награда не нашлась   |
| `count(*)`                                     | Считает такие фильмы                              |

```sql
SELECT count(*) AS movies_without_awards
FROM movies
LEFT JOIN awards ON movies.id = awards.movie
WHERE awards.id IS NULL;
```

---

## 🧪 Практика 7 — актёр с максимумом строк роли `актёр`

| Версия                      | Статус | Почему                                                    |
| --------------------------- | ------ | --------------------------------------------------------- |
| `count(DISTINCT movies.id)` | ❌      | Считает уникальные фильмы и даёт другой результат         |
| `count(*)` по `relations`   | ✅      | Курс ожидает количество строк `relations` с ролью `актёр` |
| Топ-10                      | ✅      | Удобно для проверки                                       |
| Только имя первого          | ✅      | Нужно для ответа курса                                    |

Правильный код для топ-10:

```sql
SELECT
    relations.person,
    persons.name,
    count(*) AS movies_count
FROM relations
JOIN persons ON persons.id = relations.person
WHERE relations.role = 'актёр'
GROUP BY relations.person, persons.name
ORDER BY count(*) DESC
LIMIT 10;
```

Если нужен только первый ответ:

```sql
SELECT
    persons.name
FROM relations
JOIN persons ON persons.id = relations.person
WHERE relations.role = 'актёр'
GROUP BY relations.person, persons.name
ORDER BY count(*) DESC
LIMIT 1;
```

Ожидаемый ответ:

```text
Tessa Kim
```

---

## 🧪 Практика 8 — фильм с максимумом наград `Патрик`

| SQL-часть                                 | Что делает                                 |
| ----------------------------------------- | ------------------------------------------ |
| `JOIN awards ON movies.id = awards.movie` | Соединяет фильмы и награды                 |
| `awards.award LIKE '%Патрик'`             | Берёт награды, заканчивающиеся на `Патрик` |
| `GROUP BY movies.id, movies.name`         | Группирует по фильму                       |
| `count(*) AS patricks_count`              | Считает количество таких наград            |
| `ORDER BY count(*) DESC LIMIT 1`          | Берёт фильм с максимумом                   |

```sql
SELECT
    movies.name,
    count(*) AS patricks_count
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE awards.award LIKE '%Патрик'
GROUP BY movies.id, movies.name
ORDER BY count(*) DESC
LIMIT 1;
```

Для проверки топ-10:

```sql
SELECT
    movies.id,
    movies.name,
    count(*) AS patricks_count
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE awards.award LIKE '%Патрик'
GROUP BY movies.id, movies.name
ORDER BY count(*) DESC
LIMIT 10;
```

---

## 🧪 Практика 9 — режиссёры с более чем одним Оскаром

| Версия                                | Статус        | Причина                                                |
| ------------------------------------- | ------------- | ------------------------------------------------------ |
| Внешний `SELECT count(*) FROM (...)`  | ❌             | Считает количество людей, но курс ожидает строки людей |
| Один `SELECT` с `GROUP BY` и `HAVING` | ✅             | Возвращает режиссёров и число Оскаров                  |
| `JOIN movies`                         | Не обязателен | `relations.movie` уже связывается с `awards.movie`     |
| `HAVING count(*) > 1`                 | Обязательно   | Оставляет только людей с несколькими Оскарами          |

Правильный код:

```sql
SELECT
    persons.id,
    persons.name,
    count(*)
FROM persons
JOIN relations ON persons.id = relations.person
JOIN awards ON awards.movie = relations.movie
WHERE relations.role = 'режиссёр'
  AND awards.award = 'Оскар'
GROUP BY persons.id, persons.name
HAVING count(*) > 1;
```

---

## ✅ Главное запомнить

| Тема                  | Правило                            | Пример                      |
| --------------------- | ---------------------------------- | --------------------------- |
| `JOIN`                | Показывает только совпавшие строки | Фильмы с наградами          |
| `LEFT JOIN`           | Сохраняет все строки левой таблицы | Все фильмы, даже без наград |
| `ON`                  | Условие соединения                 | `movies.id = awards.movie`  |
| `LIKE`                | Поиск по шаблону                   | `LIKE '%Патрик'`            |
| `%`                   | Любой текст любой длины            | `'%кот'`                    |
| `IS NULL`             | Проверка отсутствующей связи       | Фильмы без наград           |
| `GROUP BY` после JOIN | Считает группы связанных строк     | Награды по жанрам           |
| `count(*)`            | Считает строки результата          | Используется в `relations`  |
| `count(column)`       | Не считает `NULL`                  | Важно при `LEFT JOIN`       |
| `HAVING`              | Фильтрует группы                   | Режиссёры с `count(*) > 1`  |
| Явные имена таблиц    | Убирают неоднозначность            | `movies.id`, `persons.id`   |

---

## 🏁 Итог

| 📌 Что изучено                              | ✅ Статус |
| ------------------------------------------- | -------- |
| JOIN                                        | ✅        |
| INNER JOIN                                  | ✅        |
| LEFT OUTER JOIN                             | ✅        |
| ON                                          | ✅        |
| LIKE                                        | ✅        |
| Работа с несколькими таблицами              | ✅        |
| Соединение `movies` и `awards`              | ✅        |
| Соединение `relations`, `movies`, `persons` | ✅        |
| Поиск фильмов без наград                    | ✅        |
| Группировка после JOIN                      | ✅        |
| Подсчёт наград по жанрам                    | ✅        |
| Поиск топ-актёра                            | ✅        |
| Поиск фильма с максимумом `Патриков`        | ✅        |
| Поиск режиссёров с несколькими `Оскарами`   | ✅        |
| Исправления запросов под формат курса       | ✅        |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add lessons/sprint_20_20/keys_indexes_transactions/lesson_6_8

git commit -m "Add joins lesson practices"

git push
```

---

## ⬅️ Назад

[Вернуться к Keys, Indexes and Transactions](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
