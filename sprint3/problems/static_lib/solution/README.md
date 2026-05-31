# 📦 Static Lib — Game Model Library

## 📌 Описание

Задача `static_lib` продолжает решение `gen_objects`.

Главная цель — вынести игровую модель и связанные с ней зависимости в отдельную статическую библиотеку:

```text
game_model
```

Теперь общий код модели собирается один раз, а затем подключается к серверу через `target_link_libraries`.

---

## 🧠 Что изучается

| 📌 Тема               | 📖 Что происходит                                              | ✅ Итог                                 |
| --------------------- | -------------------------------------------------------------- | -------------------------------------- |
| Static Library        | `model.cpp` и `loot_generator.cpp` собираются в `game_model`   | Общий код вынесен отдельно             |
| CMake                 | Используется `add_library(... STATIC ...)`                     | Сборка становится чище                 |
| target_link_libraries | Сервер подключает `game_model`                                 | Нет дублирования `.cpp` файлов         |
| Game Server           | Сервер продолжает собираться и запускаться                     | Поведение не ломается                  |
| Docker-style CMake    | Основной `CMakeLists.txt` собирает сервер без локальных тестов | Проект лучше подходит для автопроверки |

---

## 📂 Структура решения

```text
static_lib/solution/
├── CMakeLists.txt
├── conanfile.txt
├── data/
├── static/
├── src/
│   ├── model.h
│   ├── model.cpp
│   ├── tagged.h
│   ├── loot_generator.h
│   ├── loot_generator.cpp
│   ├── main.cpp
│   ├── request_handler.h
│   ├── request_handler.cpp
│   └── ...
└── README.md
```

---

## 🏗️ Создание `static_lib/solution`

Задача создаётся как копия готового решения `gen_objects`:

```bash
cd ~/cppbackend/sprint3/problems

mkdir -p static_lib

rm -rf static_lib/solution

cp -r gen_objects/solution static_lib/solution

cd static_lib/solution
```

---

## 📦 conanfile.txt

Открыть:

```bash
nano conanfile.txt
```

Оставить:

```ini
[requires]
boost/1.78.0
catch2/3.1.0

[generators]
cmake
```

`Catch2` можно оставить: ревьюер или ты сам можешь захотеть собрать тесты вручную.

---

## ⚙️ CMakeLists.txt

Открыть:

```bash
nano CMakeLists.txt
```

Заменить весь файл:

```cmake
cmake_minimum_required(VERSION 3.11)

project(game_server CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Threads REQUIRED)

add_definitions(-DBOOST_LOG_STATIC_LINK)

add_library(game_model STATIC
    src/model.h
    src/model.cpp
    src/tagged.h
    src/loot_generator.h
    src/loot_generator.cpp
)

target_link_libraries(game_model PUBLIC
    Threads::Threads
)

add_executable(game_server
    src/main.cpp
    src/http_server.cpp
    src/http_server.h
    src/sdk.h
    src/boost_json.cpp
    src/json_loader.h
    src/json_loader.cpp
    src/json_serializer.h
    src/json_serializer.cpp
    src/request_handler.cpp
    src/request_handler.h
    src/ticker.h
    src/logger.cpp
    src/logger.h
    src/players.h
    src/player_tokens.h
    src/use_cases.h
    src/extra_data.h
    src/extra_data.cpp
)

target_link_libraries(game_server PRIVATE
    game_model
    ${CONAN_LIBS}
    Threads::Threads
)
```

---

## 🔍 Что изменилось в CMake

Раньше сервер напрямую собирал модель:

```cmake
add_executable(game_server
    src/model.cpp
    src/loot_generator.cpp
    ...
)
```

Если бы добавлялись тесты, пришлось бы снова писать:

```cmake
add_executable(game_server_tests
    src/model.cpp
    src/loot_generator.cpp
    tests/model-tests.cpp
)
```

То есть одни и те же файлы повторялись бы несколько раз.

Теперь общий код вынесен отдельно:

```cmake
add_library(game_model STATIC
    src/model.cpp
    src/loot_generator.cpp
)
```

А сервер просто подключает библиотеку:

```cmake
target_link_libraries(game_server PRIVATE
    game_model
)
```

---

## 🧠 Главная идея

Было:

```text
model.cpp
loot_generator.cpp
↓
повторяются в разных target
```

Стало:

```text
model.cpp + loot_generator.cpp
↓
game_model STATIC library
↓
game_server
```

Такой подход уменьшает дублирование и делает `CMakeLists.txt` чище.

---

## 🏗️ Локальная сборка

```bash
cd ~/cppbackend/sprint3/problems/static_lib/solution

rm -rf build

mkdir build

cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск сервера

Из-за `conan_basic_setup()` исполняемый файл обычно лежит здесь:

```text
build/bin/game_server
```

Поэтому запуск:

```bash
cd ~/cppbackend/sprint3/problems/static_lib/solution/build

./bin/game_server \
  --config-file ../data/config.json \
  --www-root ../static \
  --tick-period 100
```

Если вдруг папки `bin` нет, проверь:

```bash
ls
ls bin
```

и запускай по фактическому пути.

---

## 🔎 Проверка API

В другом терминале:

```bash
curl http://127.0.0.1:8080/api/v1/maps/map1
```

Ожидаемо сервер должен вернуть JSON карты, включая:

```json
"lootTypes":[...]
```

---

## 🏁 Итог

В этом задании:

```text
gen_objects
↓
static_lib
↓
game_model STATIC library
↓
game_server линкуется с game_model
```

Главный результат:

```text
игровая модель вынесена в отдельную статическую библиотеку
```

Это делает проект ближе к production-структуре, где модель, приложение, сервер и тесты собираются отдельными target.

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../../../../lessons/sprint_19_20/collision_detection_and_statistics/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
