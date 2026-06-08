# 🗄️ Lesson 7/2 — Типы данных PostgreSQL и безопасное хранение пользователей

## 📌 Описание урока

| 📌 Блок                | 📖 Что изучается                                                                                                                  | 🧠 Практическая ценность                                    | ✅ Результат                      |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------- | -------------------------------- |
| Типы PostgreSQL        | `smallint`, `integer`, `bigint`, `SERIAL`, `varchar`, `char`, `text`, `bytea`, `float4`, `float8`, `boolean`, `UUID`, `timestamp` | Позволяет правильно выбирать типы полей для хранения данных | Освоены основные типы PostgreSQL |
| Хранение пользователей | `pgcrypto`, `crypt()`, `gen_salt('bf')`, соль и хеширование                                                                       | Позволяет не хранить пароли в открытом виде                 | Пароли сохраняются как хеши      |
| Авторизация            | Проверка логина и пароля через `crypt(введённый_пароль, pass)`                                                                    | Позволяет проверить пароль без хранения исходного значения  | Освоена базовая авторизация      |

---

## 📂 Структура урока

| 📄 Файл                 | 📖 Назначение                                                         | ▶️ Как запускать                                                              |
| ----------------------- | --------------------------------------------------------------------- | ----------------------------------------------------------------------------- |
| `README.md`             | Конспект урока и описание практик                                     | Открывать как инструкцию                                                      |
| `practice_01_types.sql` | Создание таблицы `test_types` с разными типами PostgreSQL             | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_01_types.sql` |
| `practice_02_users.sql` | Создание `pgcrypto`, таблицы `users` и пользователей с хешами паролей | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_02_users.sql` |
| `practice_03_auth.sql`  | Проверка правильного и неправильного пароля через `crypt()`           | `psql -h localhost -p 30432 -U postgres -d postgres -f practice_03_auth.sql`  |

---

## 🧠 Типы данных PostgreSQL

| Тип                        | Расшифровка / смысл           | Размер / особенность                             | Где использовать                         |
| -------------------------- | ----------------------------- | ------------------------------------------------ | ---------------------------------------- |
| `smallint`                 | small integer                 | 2 байта, диапазон `-32768..32767`                | месяц, день, маленькие числа             |
| `integer`                  | целое число                   | 4 байта, диапазон примерно `-2.1 млрд..2.1 млрд` | возраст, количество, обычные числа       |
| `bigint`                   | большое целое число           | 8 байт                                           | большие счётчики, деньги, идентификаторы |
| `SERIAL`                   | `integer + sequence`          | не настоящий тип, а удобное сокращение           | автоинкрементный `id`                    |
| `varchar(n)`               | variable character            | строка переменной длины до `n` символов          | логин, имя, короткий текст               |
| `char(n)`                  | character                     | строка фиксированной длины                       | код страны, ISBN, фиксированные коды     |
| `text`                     | текст                         | практически без ограничения длины                | статьи, описания, комментарии, JSON      |
| `bytea`                    | byte array                    | бинарные данные                                  | изображения, архивы, PDF, файлы          |
| `float4`                   | floating point                | 4 байта, аналог `float`                          | приблизительные дробные значения         |
| `float8`                   | floating point                | 8 байт, аналог `double`                          | точныенее дробные значения               |
| `boolean`                  | логический тип                | `true` / `false`                                 | флаги, признаки, настройки               |
| `UUID`                     | Universally Unique Identifier | уникальный идентификатор                         | id без зависимости от `SERIAL`           |
| `timestamp`                | дата + время                  | без часового пояса                               | дата создания, время события             |
| `timestamp with time zone` | дата + время + часовой пояс   | учитывает часовой пояс                           | события пользователей из разных зон      |

---

## 🔢 SERIAL и sequence

| 📌 Конструкция   | 📖 Что делает                             | 🧠 Пример                        |
| ---------------- | ----------------------------------------- | -------------------------------- |
| `id SERIAL`      | PostgreSQL создаёт `integer` и `sequence` | `1, 2, 3, 4...`                  |
| `PRIMARY KEY`    | Делает поле уникальным и обязательным     | `id SERIAL PRIMARY KEY`          |
| `DEFAULT VALUES` | Вставляет строку с default-значениями     | sequence сам выдаёт следующий id |

```sql
CREATE TABLE books(
    id SERIAL PRIMARY KEY
);

INSERT INTO books DEFAULT VALUES;
INSERT INTO books DEFAULT VALUES;
INSERT INTO books DEFAULT VALUES;
```

---

## 🧪 Практика №1 — типы данных

| 📌 Шаг | SQL / команда                      | 📖 Что делает                         |
| ------ | ---------------------------------- | ------------------------------------- |
| 1      | `DROP TABLE IF EXISTS test_types;` | Удаляет старую таблицу, если она есть |
| 2      | `CREATE TABLE test_types (...)`    | Создаёт таблицу с разными типами      |
| 3      | `\d test_types`                    | Показывает структуру таблицы          |

Файл `practice_01_types.sql`:

```sql
DROP TABLE IF EXISTS test_types;

CREATE TABLE test_types(
    id SERIAL PRIMARY KEY,
    age integer,
    month smallint,
    balance bigint,
    name varchar(100),
    description text,
    image bytea,
    rating float8,
    active boolean,
    created_at timestamp DEFAULT now()
);
```

Запуск:

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-d postgres \
-f practice_01_types.sql
```

Проверка внутри `psql`:

```sql
\d test_types
```

---

## 🔐 Безопасное хранение пользователей

| ❌ Плохо                                    | ✅ Хорошо                          | Почему                                                         |
| ------------------------------------------ | --------------------------------- | -------------------------------------------------------------- |
| `password text`                            | `pass text` с хешем               | В базе не хранится исходный пароль                             |
| `password = '123456'`                      | `crypt(password, gen_salt('bf'))` | При утечке БД нельзя сразу увидеть пароль                      |
| одинаковый пароль даёт одинаковое значение | соль делает хеши разными          | одинаковые пароли у разных пользователей не выглядят одинаково |

---

## 🧂 Хеширование и соль

| Термин                  | Расшифровка                        | Значение                               |
| ----------------------- | ---------------------------------- | -------------------------------------- |
| hash function           | хеш-функция                        | Необратимо превращает пароль в хеш     |
| salt                    | соль                               | Случайная строка перед хешированием    |
| `pgcrypto`              | PostgreSQL Cryptographic Extension | Расширение PostgreSQL для криптографии |
| `gen_salt('bf')`        | generate salt, Blowfish            | Создаёт соль для алгоритма Blowfish    |
| `crypt(password, salt)` | хеширование                        | Создаёт хеш пароля                     |

Схема:

```text
пароль
+
соль
↓
crypt()
↓
хеш
```

---

## 🧪 Практика №2 — пользователи и хеши

| 📌 Шаг | SQL                                        | 📖 Что делает                        |
| ------ | ------------------------------------------ | ------------------------------------ |
| 1      | `CREATE EXTENSION IF NOT EXISTS pgcrypto;` | Подключает криптографические функции |
| 2      | `DROP TABLE IF EXISTS users;`              | Удаляет старую таблицу пользователей |
| 3      | `CREATE TABLE users (...)`                 | Создаёт таблицу пользователей        |
| 4      | `crypt('qwerty', gen_salt('bf'))`          | Сохраняет не пароль, а хеш           |
| 5      | `SELECT login, pass FROM users;`           | Показывает, что хеши разные          |

Файл `practice_02_users.sql`:

```sql
CREATE EXTENSION IF NOT EXISTS pgcrypto;

DROP TABLE IF EXISTS users;

CREATE TABLE users(
    id SERIAL PRIMARY KEY,
    login varchar(100) NOT NULL,
    pass text NOT NULL,
    registered timestamp DEFAULT now()
);

INSERT INTO users(login, pass)
VALUES(
    'Ivan',
    crypt('qwerty', gen_salt('bf'))
);

INSERT INTO users(login, pass)
VALUES(
    'Petr',
    crypt('qwerty', gen_salt('bf'))
);
```

Запуск:

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-d postgres \
-f practice_02_users.sql
```

Проверка:

```sql
SELECT login, pass FROM users;
```

---

## 🔑 Практика №3 — авторизация

| Проверка            | SQL                                           | Ожидаемый результат            |
| ------------------- | --------------------------------------------- | ------------------------------ |
| Правильный пароль   | `login='Ivan' AND pass=crypt('qwerty', pass)` | Возвращается `id` пользователя |
| Неправильный пароль | `login='Ivan' AND pass=crypt('123456', pass)` | `0 rows`                       |

Файл `practice_03_auth.sql`:

```sql
SELECT id
FROM users
WHERE login='Ivan'
AND pass=crypt('qwerty', pass);

SELECT id
FROM users
WHERE login='Ivan'
AND pass=crypt('123456', pass);
```

Запуск:

```bash
psql \
-h localhost \
-p 30432 \
-U postgres \
-d postgres \
-f practice_03_auth.sql
```

---

## 🧠 Как работает проверка пароля

| 📌 Элемент                    | 📖 Что происходит                                      |
| ----------------------------- | ------------------------------------------------------ |
| `pass`                        | Хранит старый хеш вместе с солью                       |
| `crypt('qwerty', pass)`       | Берёт соль из старого хеша и хеширует введённый пароль |
| Новый хеш совпал со старым    | Пароль правильный                                      |
| Новый хеш не совпал со старым | Пароль неправильный                                    |

---

## ✅ Главное запомнить

| Для чего            | Использовать                            |
| ------------------- | --------------------------------------- |
| Текст               | `text`, `varchar`                       |
| Идентификаторы      | `SERIAL`, `UUID`                        |
| Время               | `timestamp`, `timestamp with time zone` |
| Файлы               | `bytea`                                 |
| Логические значения | `boolean`                               |
| Большие числа       | `bigint`                                |
| Пароли              | `crypt(password, gen_salt('bf'))`       |
| Авторизация         | `pass = crypt(введённый_пароль, pass)`  |

---

## 🏁 Итог

| 📌 Что изучено              | ✅ Статус |
| --------------------------- | -------- |
| Основные типы PostgreSQL    | ✅        |
| `SERIAL` и `sequence`       | ✅        |
| `varchar`, `char`, `text`   | ✅        |
| `bytea` для бинарных данных | ✅        |
| `timestamp` для времени     | ✅        |
| `UUID` для идентификаторов  | ✅        |
| `pgcrypto`                  | ✅        |
| `crypt()`                   | ✅        |
| `gen_salt('bf')`            | ✅        |
| Хеширование паролей         | ✅        |
| Проверка авторизации        | ✅        |

---

## ⬅️ Назад

[Вернуться к Keys, Indexes and Transactions](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
