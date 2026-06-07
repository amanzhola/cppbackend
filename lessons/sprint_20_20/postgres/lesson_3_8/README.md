# 🗄 PostgreSQL — Lesson 3/8

## 📌 Тема урока

| 📌 Тема                 | 📖 Что изучается                                 | 🧠 Практический результат                                       |
| ----------------------- | ------------------------------------------------ | --------------------------------------------------------------- |
| Реляционные базы данных | Таблицы, ключи, связи, нормализация, SQL-запросы | Освоены основы проектирования реляционных баз данных PostgreSQL |

---

## 🧠 Основные понятия

| Термин              | Значение                |
| ------------------- | ----------------------- |
| Relation            | Отношение (таблица)     |
| Row                 | Строка таблицы          |
| Column              | Столбец таблицы         |
| Relational Database | Реляционная база данных |
| Primary Key         | Первичный ключ          |
| Foreign Key         | Внешний ключ            |

---

## 📊 Пример таблицы

```text
cars

id | name        | color
1  | Zaporozhets | red
2  | Lada        | blue
```

| Элемент | Значение        |
| ------- | --------------- |
| Таблица | cars            |
| Столбцы | id, name, color |
| Строка  | одна машина     |

---

## 🔑 Primary Key

Primary Key — уникальный идентификатор строки.

Пример:

```text
engines

id | fuel   | power
1  | petrol | 164
2  | diesel | 128
```

| Поле | Назначение     |
| ---- | -------------- |
| id   | Первичный ключ |

---

## 🔗 Foreign Key

Foreign Key используется для связи таблиц.

Пример:

```text
cars.engine_id -> engines.id
```

Машина хранит ссылку на двигатель через идентификатор.

---

## 🚗 Создание базы данных

Подключение:

```bash
psql -h localhost -p 30432 -U postgres -W
```

Создание базы:

```sql
CREATE DATABASE cars_db;
```

Подключение:

```sql
\c cars_db
```

---

## ⚙️ Таблица двигателей

```sql
CREATE TABLE engines (
    id INTEGER PRIMARY KEY,
    fuel TEXT NOT NULL,
    power INTEGER NOT NULL
);
```

| Поле  | Тип     |
| ----- | ------- |
| id    | INTEGER |
| fuel  | TEXT    |
| power | INTEGER |

---

## 🛞 Таблица шин

```sql
CREATE TABLE tires (
    id INTEGER PRIMARY KEY,
    season TEXT NOT NULL
);
```

---

## 🚙 Таблица автомобилей

```sql
CREATE TABLE cars (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    engine_id INTEGER NOT NULL REFERENCES engines(id),
    color TEXT NOT NULL,
    tires_id INTEGER NOT NULL REFERENCES tires(id)
);
```

Связи:

```text
cars.engine_id -> engines.id
cars.tires_id  -> tires.id
```

---

## ➕ Добавление данных

Двигатели:

```sql
INSERT INTO engines (id, fuel, power)
VALUES
    (0, 'petrol', 164),
    (10, 'diesel', 128);
```

Шины:

```sql
INSERT INTO tires (id, season)
VALUES
    (0, 'winter'),
    (1, 'all-season');
```

Автомобили:

```sql
INSERT INTO cars (id, name, engine_id, color, tires_id)
VALUES
    (0, 'Zaporozhets', 0, 'red', 0),
    (1, 'Lada', 10, 'blue', 1);
```

---

## 🔍 Проверка данных

Показать все автомобили:

```sql
SELECT * FROM cars;
```

---

## 🔗 JOIN-запрос

```sql
SELECT
    cars.id,
    cars.name,
    cars.color,
    engines.fuel,
    engines.power,
    tires.season
FROM cars
JOIN engines ON cars.engine_id = engines.id
JOIN tires ON cars.tires_id = tires.id;
```

Результат:

```text
id | name        | color | fuel   | power | season
0  | Zaporozhets | red   | petrol | 164   | winter
1  | Lada        | blue  | diesel | 128   | all-season
```

---

## 📚 Нормализация данных

| Нормальная форма | Основная идея                                    |
| ---------------- | ------------------------------------------------ |
| 1НФ              | В каждой ячейке одно значение                    |
| 2НФ              | Зависимость от всего ключа                       |
| 3НФ              | Отсутствие зависимостей между неключевыми полями |

---

## 1️⃣ Первая нормальная форма (1НФ)

Нельзя хранить списки внутри одной ячейки.

Плохо:

```text
Lada | petrol, diesel
```

Хорошо:

```text
Lada | petrol
Lada | diesel
```

---

## 2️⃣ Вторая нормальная форма (2НФ)

Все неключевые поля должны зависеть от полного составного ключа.

Проблема:

```text
car_name | engine_type | power
```

Поле `power` зависит только от двигателя.

Поэтому двигатели выносятся в отдельную таблицу.

---

## 3️⃣ Третья нормальная форма (3НФ)

Нельзя хранить зависимости между неключевыми полями.

Плохо:

```text
engine_type -> power
```

Правильно:

```text
cars
id | name | engine_id

engines
id | type | power
```

---

## 🛠 Полезные команды PostgreSQL

Показать таблицы:

```sql
\dt
```

Структура таблицы:

```sql
\d cars
```

Список баз:

```sql
\l
```

Список ролей:

```sql
\du
```

---

## 🧹 Очистка после практики

Подключиться к системной базе:

```sql
\c postgres
```

Удалить базу:

```sql
DROP DATABASE cars_db;
```

---

## ✅ Результат

| Что изучено                           | Статус |
| ------------------------------------- | ------ |
| Реляционная модель данных             | ✅      |
| Таблицы PostgreSQL                    | ✅      |
| Primary Key                           | ✅      |
| Foreign Key                           | ✅      |
| JOIN-запросы                          | ✅      |
| Нормализация данных                   | ✅      |
| 1НФ                                   | ✅      |
| 2НФ                                   | ✅      |
| 3НФ                                   | ✅      |
| Проектирование связей между таблицами | ✅      |

---

## ⬅️ Назад

[Вернуться к PostgreSQL](../README.md)

[Вернуться в Sprint 4](../../../README.md)
