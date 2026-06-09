# 🧵 Lesson 3/7 — Добавляем БД в игру

## 📌 Описание урока

| 📌 Блок                      | 📖 Что изучается                                                                                   | 🧠 Практическая ценность                                                     | ✅ Результат                                |
| ---------------------------- | -------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------- | ------------------------------------------ |
| Потокобезопасность `libpqxx` | `pqxx::connection`, `pqxx::work`, `pqxx::read_transaction`, правило «один поток — одно соединение» | Позволяет безопасно работать с PostgreSQL из многопоточного игрового сервера | Освоены правила безопасной работы с `pqxx` |
| Время подключения            | Измерение создания соединения, транзакции, запроса и закрытия соединения                           | Показывает, почему создавать соединение на каждый запрос дорого              | Освоено измерение стоимости подключения    |
| ConnectionPool               | Пул соединений, `ConnectionWrapper`, `RAII`, `mutex`, `condition_variable`                         | Позволяет переиспользовать готовые подключения к PostgreSQL                  | Подготовлена основа для backend-сервера    |
| Deadlock                     | Зависание при повторном получении соединения из пула размера `1`                                   | Помогает избегать взаимных блокировок                                        | Освоено безопасное использование пула      |

---

## 📂 Структура урока

| 📄 Файл / папка                      | 📖 Назначение                                                   | ▶️ Запуск / действие                                              |
| ------------------------------------ | --------------------------------------------------------------- | ----------------------------------------------------------------- |
| `README.md`                          | Конспект урока и описание практик                               | Читать как инструкцию                                             |
| `practice_01_thread_safety_notes.md` | Заметки по потокобезопасности `pqxx::connection` и `pqxx::work` | Открыть как конспект                                              |
| `practice_02_connection_timing/`     | C++ пример измерения времени подключения к БД                   | Собрать через Conan + CMake и запустить `connection_timing`       |
| `practice_03_connection_pool/`       | C++ пример пула соединений PostgreSQL                           | Собрать через Conan + CMake и запустить `connection_pool_example` |
| `practice_04_deadlock_example.md`    | Разбор deadlock при пуле размера `1`                            | Открыть как конспект                                              |

---

## 🧠 Главная идея

| Правило                                   | Что означает                                                                    | Почему важно                                    |
| ----------------------------------------- | ------------------------------------------------------------------------------- | ----------------------------------------------- |
| Один поток → одно соединение              | `pqxx::connection` нельзя безопасно использовать одновременно из разных потоков | Иначе возможны гонки и неопределённое поведение |
| Один поток → одна транзакция              | `pqxx::work` нельзя шарить между потоками                                       | Транзакция привязана к конкретному соединению   |
| Соединение дорого создавать часто         | Подключение к PostgreSQL требует времени                                        | Нужен `ConnectionPool`                          |
| Пул соединений переиспользует подключения | Соединение берётся, используется и возвращается                                 | Сервер работает быстрее и стабильнее            |
| RAII возвращает соединение автоматически  | Деструктор `ConnectionWrapper` вызывает `ReturnConnection`                      | Меньше риска забыть вернуть соединение          |

---

## 📚 Расшифровки

| Термин                   | Расшифровка                            | Значение                                                          |
| ------------------------ | -------------------------------------- | ----------------------------------------------------------------- |
| `libpqxx`                | library PostgreSQL Query C++           | C++ библиотека для работы с PostgreSQL                            |
| `pqxx::connection`       | connection                             | Соединение с PostgreSQL                                           |
| `pqxx::work`             | write transaction                      | Транзакция для записи                                             |
| `pqxx::read_transaction` | read transaction                       | Транзакция для чтения                                             |
| `RAII`                   | Resource Acquisition Is Initialization | Ресурс захватывается в конструкторе и освобождается в деструкторе |
| `mutex`                  | mutual exclusion                       | Взаимное исключение                                               |
| `condition_variable`     | condition variable                     | Механизм ожидания условия                                         |
| `deadlock`               | dead lock                              | Взаимная блокировка / зависание                                   |

---

## 🧪 Практика 1 — потокобезопасность pqxx

| Ситуация                                             | Потокобезопасно | Причина                                                   |
| ---------------------------------------------------- | --------------- | --------------------------------------------------------- |
| Один `pqxx::work` на два потока                      | ❌               | Два потока одновременно используют один объект транзакции |
| Два `pqxx::work`, но один `pqxx::connection`         | ❌               | Соединение используется одновременно                      |
| Два потока и два разных `pqxx::connection`           | ✅               | У каждого потока своё соединение                          |
| Временный `std::jthread`, который сразу уничтожается | ✅               | Деструктор `jthread` делает `join()` в конце строки       |
| Главный поток и `t1` используют один `connection`    | ❌               | Одновременная работа через один `conn`                    |

Файл заметок:

```bash
cd ~/cppbackend/lessons/sprint_20_20/diploma/lesson_3_7

mv practice_01_thread_safety_notes.cpp practice_01_thread_safety_notes.md

nano practice_01_thread_safety_notes.md
```

Главное правило:

```text
Один поток → одно pqxx::connection.
Один поток → один pqxx::work.
```

---

## ❌ Небезопасно: один work на два потока

| Что происходит                      | Почему плохо                             |
| ----------------------------------- | ---------------------------------------- |
| Создан один `pqxx::work work{conn}` | Транзакция одна                          |
| `t1` вызывает `work.query()`        | Первый поток использует транзакцию       |
| `t2` вызывает `work.query()`        | Второй поток использует ту же транзакцию |
| Оба могут вызвать `work.commit()`   | Неопределённое поведение                 |

```cpp
pqxx::connection conn{std::getenv("DB_URL")};
pqxx::work work{conn};

std::jthread t1{[&work] {
    work.query(...);
    work.commit();
}};

std::jthread t2{[&work] {
    work.query(...);
    work.commit();
}};
```

---

## ❌ Небезопасно: разные work, но один conn

| Что происходит                 | Почему плохо                                   |
| ------------------------------ | ---------------------------------------------- |
| `work` использует `conn`       | Первое использование соединения                |
| `work2` тоже использует `conn` | Второе использование того же соединения        |
| Потоки работают параллельно    | Один `pqxx::connection` шарится между потоками |

```cpp
pqxx::connection conn{std::getenv("DB_URL")};

pqxx::work work{conn};
pqxx::work work2{conn};
```

---

## ✅ Безопасно: каждому потоку своё соединение

| Поток | Соединение | Транзакция | Статус |
| ----- | ---------- | ---------- | ------ |
| `t1`  | `conn`     | `work`     | ✅      |
| `t2`  | `conn2`    | `work2`    | ✅      |

```cpp
pqxx::connection conn{std::getenv("DB_URL")};
pqxx::connection conn2{std::getenv("DB_URL")};

pqxx::work work{conn};
pqxx::work work2{conn2};
```

---

## ⏱ Практика 2 — измеряем время соединения

| Что измеряется        | Переменная времени                      | Зачем нужно                         |
| --------------------- | --------------------------------------- | ----------------------------------- |
| Создание соединения   | `conn_time - start_time`                | Понять стоимость подключения        |
| Создание транзакции   | `tx_construction_time - conn_time`      | Понять стоимость `read_transaction` |
| Выполнение запроса    | `query_end_time - tx_construction_time` | Понять стоимость `SELECT 1`         |
| Разрушение транзакции | `tx_end_time - query_end_time`          | Понять накладные расходы транзакции |
| Закрытие соединения   | `conn_end_time - tx_end_time`           | Понять стоимость закрытия           |
| Полное время          | `conn_end_time - start_time`            | Полная стоимость одного подключения |

Создание проекта:

```bash
cd ~/cppbackend/lessons/sprint_20_20/diploma/lesson_3_7

mkdir -p practice_02_connection_timing
cd practice_02_connection_timing
```

`conanfile.txt`:

```ini
[requires]
libpqxx/7.7.4

[generators]
cmake_multi
```

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.11)

project(connection_timing CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(connection_timing main.cpp)

target_link_libraries(connection_timing CONAN_PKG::libpqxx)
```

Сборка:

```bash
mkdir build
cd build

conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

Запуск:

```bash
export DB_URL="postgres://postgres:postgres@localhost:30432/postgres"

./bin/connection_timing
```

---

## 🔌 Практика 3 — ConnectionPool

| Компонент                 | Назначение                             | Почему нужен                                   |
| ------------------------- | -------------------------------------- | ---------------------------------------------- |
| `ConnectionPool`          | Хранит набор готовых соединений        | Не создавать соединение каждый раз             |
| `ConnectionWrapper`       | RAII-обёртка над соединением           | Автоматически возвращает соединение в пул      |
| `std::mutex`              | Защищает `pool_` и `used_connections_` | Нельзя менять пул из двух потоков одновременно |
| `std::condition_variable` | Усыпляет поток, если соединений нет    | Поток ждёт освобождения соединения             |
| `ReturnConnection`        | Возвращает соединение обратно          | Делает соединение доступным для другого потока |
| `GetConnection`           | Выдаёт свободное соединение            | Основной метод работы с пулом                  |

Создание проекта:

```bash
cd ~/cppbackend/lessons/sprint_20_20/diploma/lesson_3_7

mkdir -p practice_03_connection_pool
cd practice_03_connection_pool

cp ../practice_02_connection_timing/conanfile.txt .
cp ../practice_02_connection_timing/CMakeLists.txt .
sed -i 's/connection_timing/connection_pool_example/g' CMakeLists.txt
```

Сборка:

```bash
mkdir build
cd build

conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .
```

Запуск:

```bash
export DB_URL="postgres://postgres:postgres@localhost:30432/postgres"

./bin/connection_pool_example
```

---

## 🧱 Как работает ConnectionPool

| Шаг | Что происходит                                | Где в коде                              |
| --- | --------------------------------------------- | --------------------------------------- |
| 1   | Пул создаёт `capacity` соединений             | Конструктор `ConnectionPool`            |
| 2   | Поток вызывает `GetConnection()`              | Метод `GetConnection`                   |
| 3   | Если соединение есть, оно выдаётся            | `std::move(pool_[used_connections_++])` |
| 4   | Если соединений нет, поток ждёт               | `cond_var_.wait(...)`                   |
| 5   | `ConnectionWrapper` живёт в области видимости | RAII                                    |
| 6   | При разрушении wrapper возвращает соединение  | `~ConnectionWrapper()`                  |
| 7   | Другой поток просыпается                      | `cond_var_.notify_one()`                |

---

## 🔐 mutex и condition_variable

| Механизм                  | Расшифровка           | Что делает                                          |
| ------------------------- | --------------------- | --------------------------------------------------- |
| `std::mutex`              | mutual exclusion      | Не даёт двум потокам одновременно менять пул        |
| `std::lock_guard`         | guard = охрана        | Автоматически держит блокировку в области видимости |
| `std::unique_lock`        | уникальная блокировка | Нужна для `condition_variable::wait`                |
| `std::condition_variable` | переменная условия    | Позволяет ждать свободное соединение                |
| `notify_one()`            | уведомить один поток  | Будит один ожидающий поток                          |

---

## 🧨 Практика 4 — deadlock в ConnectionPool

| Ситуация                    | Почему возникает deadlock                 |
| --------------------------- | ----------------------------------------- |
| Размер пула `1`             | В пуле только одно соединение             |
| `conn1 = GetConnection()`   | Первое соединение забрано                 |
| `conn2 = GetConnection()`   | Второй запрос ждёт свободное соединение   |
| `conn1` ещё жив             | Соединение не вернулось в пул             |
| Выполнение стоит на `conn2` | До разрушения `conn1` программа не дойдёт |

Переименовать файл:

```bash
cd ~/cppbackend/lessons/sprint_20_20/diploma/lesson_3_7

mv practice_04_deadlock_example.cpp practice_04_deadlock_example.md

nano practice_04_deadlock_example.md
```

Неправильно:

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

auto conn1 = conn_pool.GetConnection();

auto conn2 = conn_pool.GetConnection();
```

Результат:

```text
Программа зависнет.
```

---

## ✅ Как избежать deadlock

| Способ                               | Код                                           | Почему работает                        |
| ------------------------------------ | --------------------------------------------- | -------------------------------------- |
| Область видимости                    | `{ auto conn1 = conn_pool.GetConnection(); }` | `conn1` уничтожится до запроса `conn2` |
| `std::optional::reset()`             | `conn1.reset();`                              | Явно уничтожает wrapper                |
| Размер пула больше                   | `ConnectionPool conn_pool{10, ...}`           | Есть свободные соединения              |
| Не держать соединение дольше нужного | Минимизировать scope                          | Быстрее возвращает ресурс              |

Правильно:

```cpp
ConnectionPool conn_pool{1, [db_url] {
    return std::make_shared<pqxx::connection>(db_url);
}};

{
    auto conn1 = conn_pool.GetConnection();

    // работа с conn1
}

auto conn2 = conn_pool.GetConnection();

// работа с conn2
```

---

## ✅ Главное запомнить

| Тема                     | Правило                                                             |
| ------------------------ | ------------------------------------------------------------------- |
| `pqxx::connection`       | Нельзя использовать одновременно из разных потоков                  |
| `pqxx::work`             | Нельзя шарить между потоками                                        |
| Потокобезопасность       | Один поток — одно соединение                                        |
| Подключение к PostgreSQL | Дорого создавать каждый раз                                         |
| `ConnectionPool`         | Переиспользует готовые соединения                                   |
| `ConnectionWrapper`      | Возвращает соединение в пул через RAII                              |
| `mutex`                  | Защищает внутреннее состояние пула                                  |
| `condition_variable`     | Ждёт свободного соединения                                          |
| Deadlock                 | Может возникнуть, если взять второе соединение, не освободив первое |
| Scope                    | Лучший способ управлять временем жизни соединения                   |

---

## 🏁 Итог

| 📌 Что сделано                                | ✅ Статус |
| --------------------------------------------- | -------- |
| Создан `lesson_3_7`                           | ✅        |
| Создан конспект по потокобезопасности         | ✅        |
| Измерено время подключения к PostgreSQL       | ✅        |
| Создан пример `ConnectionPool`                | ✅        |
| Разобран `ConnectionWrapper`                  | ✅        |
| Разобран RAII                                 | ✅        |
| Разобраны `mutex` и `condition_variable`      | ✅        |
| Разобран deadlock при пуле размера 1          | ✅        |
| Подготовлена основа для подключения БД к игре | ✅        |

---

## 🚀 Git

```bash
cd ~/cppbackend

git add lessons/sprint_20_20/diploma/lesson_3_7

git commit -m "Add database connection pool lesson practices"

git push
```

---

## ⬅️ Назад

[Вернуться к Diploma](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)

