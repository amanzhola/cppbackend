# Практика 1 — потокобезопасность pqxx

## Главное правило

В libpqxx:

* один поток → одно соединение (`pqxx::connection`);
* один поток → одна транзакция (`pqxx::work`).

---

# Небезопасно: один `pqxx::work` на два потока

```cpp
pqxx::connection conn{std::getenv("DB_URL")};
pqxx::work work{conn};

std::jthread t1{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

std::jthread t2{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};
```

Почему плохо:

* один объект `work`;
* два потока;
* оба потока одновременно вызывают:

```cpp
work.query(...);
work.commit();
```

Результат:

```text
Не потокобезопасно.
Неопределённое поведение.
```

---

# Небезопасно: разные транзакции, но одно соединение

```cpp
pqxx::connection conn{std::getenv("DB_URL")};

pqxx::work work{conn};

std::jthread t1{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

pqxx::work work2{conn};

std::jthread t2{[&work2]{
    try {
        work2.query(...);
        work2.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};
```

Почему плохо:

* `work` и `work2` разные;
* но оба используют один `conn`;
* соединение `conn` используется одновременно.

Результат:

```text
Не потокобезопасно.
```

---

# Безопасно: каждому потоку своё соединение

```cpp
pqxx::connection conn{std::getenv("DB_URL")};
pqxx::connection conn2{std::getenv("DB_URL")};

pqxx::work work{conn};
pqxx::work work2{conn2};

std::jthread t1{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

std::jthread t2{[&work2]{
    try {
        work2.query(...);
        work2.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};
```

Почему безопасно:

```text
Поток 1 использует conn.
Поток 2 использует conn2.

Это два независимых мира libpqxx.
```

Результат:

```text
Потокобезопасно.
```

---

# Особый случай: временный std::jthread

```cpp
pqxx::connection conn{std::getenv("DB_URL")};

pqxx::work work{conn};

// временный поток
std::jthread{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

pqxx::work work2{conn};

std::jthread t2{[&work2]{
    try {
        work2.query(...);
        work2.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};
```

Почему это безопасно:

Временный объект

```cpp
std::jthread{ ... };
```

уничтожается в конце строки.

Деструктор `std::jthread` вызывает:

```cpp
join()
```

Поэтому:

1. первый поток полностью завершается;
2. только потом создаётся `work2`.

Одновременной работы с одним `conn` нет.

Результат:

```text
Потокобезопасно.
```

---

# Небезопасно: главный поток использует conn одновременно с t1

```cpp
pqxx::connection conn{std::getenv("DB_URL")};
pqxx::connection conn2{std::getenv("DB_URL")};

pqxx::work work{conn};
pqxx::work work2{conn2};

std::jthread t1{[&work] {
    try {
        work.query(...);
        work.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

std::jthread t2{[&work2]{
    try {
        work2.query(...);
        work2.commit();
    } catch (const std::exception& e) {
        std::osyncstream(std::cerr) << e.what() << std::endl;
    }
}};

pqxx::work work3{conn};
work3.commit();
```

Проблема:

* поток `t1` работает через `conn`;
* главный поток тоже использует `conn`.

Получается:

```text
t1   -> conn
main -> conn
```

Одновременно.

Результат:

```text
Не потокобезопасно.
```

---

# Сводная таблица

| Ситуация                                             | Потокобезопасно |
| ---------------------------------------------------- | --------------- |
| Один `work` на два потока                            | ❌               |
| Два `work`, но один `conn`                           | ❌               |
| Два потока и два `conn`                              | ✅               |
| Временный `std::jthread`, который сразу уничтожается | ✅               |
| Главный поток и t1 используют один `conn`            | ❌               |

---

# Главное правило libpqxx

```text
Один поток → одно pqxx::connection.

Один поток → один pqxx::work.
```
