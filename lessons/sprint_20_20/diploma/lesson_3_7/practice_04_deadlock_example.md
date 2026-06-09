# Практика 4 — дедлок в ConnectionPool

## Что такое deadlock

`deadlock`:

```text
dead = мёртвый
lock = блокировка
```

Перевод:

```text
взаимная блокировка
```

Это ситуация, когда программа ждёт событие, которое уже не может произойти.

---

# Пример дедлока

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

auto conn1 = conn_pool.GetConnection();

// ДЕДЛОК:
// пул содержит только одно соединение,
// а conn1 всё ещё жив.
auto conn2 = conn_pool.GetConnection();
```

## Почему программа зависнет

Пул создан с размером `1`:

```cpp
ConnectionPool conn_pool{1, ...};
```

Значит внутри пула есть только одно соединение.

Первая строка:

```cpp
auto conn1 = conn_pool.GetConnection();
```

забирает это единственное соединение.

Теперь пул пустой.

Дальше выполняется:

```cpp
auto conn2 = conn_pool.GetConnection();
```

Но свободных соединений нет.

`GetConnection()` начинает ждать.

Проблема в том, что `conn1` ещё жив.

Соединение вернётся в пул только тогда, когда `conn1` разрушится.

Но выполнение программы стоит на строке:

```cpp
auto conn2 = conn_pool.GetConnection();
```

До конца области видимости `conn1` программа не дойдёт.

Получается замкнутый круг:

```text
conn2 ждёт свободное соединение
conn1 освободит соединение только позже
но до этого места выполнение не доходит
```

Итог:

```text
программа зависает
```

---

# Неправильный вариант

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

auto conn1 = conn_pool.GetConnection();

auto conn2 = conn_pool.GetConnection();
```

Почему плохо:

```text
размер пула = 1
conn1 забрал единственное соединение
conn2 ждёт второе соединение
второго соединения нет
conn1 ещё не разрушен
```

Результат:

```text
deadlock
```

---

# Правильный вариант через область видимости

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

{
    auto conn1 = conn_pool.GetConnection();

    // работа с conn1
}

// здесь conn1 разрушен
// соединение вернулось обратно в пул

auto conn2 = conn_pool.GetConnection();

// работа с conn2
```

Почему это работает:

```text
conn1 живёт только внутри блока { ... }
после закрывающей скобки conn1 уничтожается
деструктор возвращает соединение в пул
после этого conn2 может взять это же соединение
```

---

# Что делает деструктор ConnectionWrapper

В пуле соединение возвращается примерно так:

```cpp
~ConnectionWrapper() {
    if (conn_) {
        pool_->ReturnConnection(std::move(conn_));
    }
}
```

Это значит:

```text
когда переменная conn1 уничтожается,
соединение автоматически возвращается в пул
```

Поэтому область видимости `{ ... }` важна.

---

# Ещё один правильный вариант: явно освободить conn1

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

std::optional<ConnectionPool::ConnectionWrapper> conn1;

conn1.emplace(conn_pool.GetConnection());

// работа с conn1

conn1.reset();

// соединение вернулось в пул

auto conn2 = conn_pool.GetConnection();
```

Но проще использовать блок:

```cpp
{
    auto conn1 = conn_pool.GetConnection();
}
```

---

# Главное правило

```text
Если пул содержит 1 соединение,
нельзя одновременно держать conn1 и просить conn2.
```

Нужно сначала освободить первое соединение:

```text
получил conn1
поработал
conn1 разрушился
получил conn2
```

---

# Короткий ответ для квиза

```cpp
ConnectionPool conn_pool{1, ...};

auto conn1 = conn_pool.GetConnection();
auto conn2 = conn_pool.GetConnection();
```

Результат:

```text
Программа зависнет.
```
