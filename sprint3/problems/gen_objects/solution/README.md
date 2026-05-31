# 🎁 Gen Objects — Lost Objects Generation

## 📌 Описание

Задача `gen_objects` добавляет в игровой сервер генерацию потерянных объектов.

В основе решения используется рабочий сервер из `sprint2/problems/command_line/solution`, а из `gen_objects/precode` берутся `loot_generator` и тесты генератора. В этой задаче модель игры расширяется объектами `lostObjects`, чтением `lootGeneratorConfig`, выдачей `lootTypes` для карты и отображением потерянных объектов в `/api/v1/game/state`.

---

## 🧠 Что изучается

| 📌 Тема          | 📖 Что реализуется                                                           | ✅ Результат                                        |
| ---------------- | ---------------------------------------------------------------------------- | -------------------------------------------------- |
| `loot_generator` | Генератор рассчитывает, сколько объектов должно появиться за прошедшее время | В игре появляются потерянные объекты               |
| `LostObject`     | Объект хранит `id`, `type` и `position`                                      | Сервер может хранить найденные предметы            |
| `lootTypes`      | JSON-описание типов объектов читается из `config.json`                       | Клиент получает описание предметов карты           |
| `lostObjects`    | Состояние объектов отдаётся через `/api/v1/game/state`                       | Игрок видит объекты на карте                       |
| `extra_data`     | Хранит JSON-данные, которые не относятся напрямую к модели                   | Модель не засоряется frontend-данными              |
| Catch2           | Проверяются генератор и модель                                               | Логика покрыта тестами                             |
| API-проверка     | Проверка через `curl`                                                        | Можно убедиться, что сервер отдаёт правильный JSON |

---

## 📂 Структура решения

```text
solution/
├── CMakeLists.txt
├── conanfile.txt
├── data/
│   └── config.json
├── static/
├── src/
│   ├── extra_data.h
│   ├── extra_data.cpp
│   ├── loot_generator.h
│   ├── loot_generator.cpp
│   ├── model.h
│   ├── model.cpp
│   ├── json_loader.h
│   ├── json_loader.cpp
│   ├── json_serializer.h
│   ├── json_serializer.cpp
│   ├── request_handler.h
│   ├── request_handler.cpp
│   └── ...
└── tests/
    ├── loot_generator_tests.cpp
    └── model-tests.cpp
```

---

## 🏗️ Подготовка solution

```bash
cd ~/cppbackend/sprint3/problems/gen_objects

rm -rf solution/*

cp -r ~/cppbackend/sprint2/problems/command_line/solution/* solution/

cp precode/src/loot_generator.h solution/src/
cp precode/src/loot_generator.cpp solution/src/

mkdir -p solution/tests
cp precode/tests/loot_generator_tests.cpp solution/tests/

cp -r precode/static solution/
cp precode/data/config.json solution/data/config.json

cd solution
```

---

## 📦 conanfile.txt

Используем Boost для сервера и Catch2 для тестов:

```ini
[requires]
boost/1.78.0
catch2/3.1.0

[generators]
cmake_multi
```

Важно использовать `cmake_multi`, чтобы можно было линковать тесты через `CONAN_PKG::catch2`.

---

## ⚙️ CMakeLists.txt

Верх файла должен использовать `conanbuildinfo_multi.cmake`:

```cmake
include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)
```

Для тестов важно линковать именно Catch2, а не весь `${CONAN_LIBS}`:

```cmake
enable_testing()

add_executable(game_server_tests
    src/model.h
    src/model.cpp
    src/tagged.h
    src/loot_generator.h
    src/loot_generator.cpp
    tests/loot_generator_tests.cpp
    tests/model-tests.cpp
)

target_link_libraries(game_server_tests PRIVATE
    CONAN_PKG::catch2
    Threads::Threads
)

add_test(NAME game_server_tests COMMAND game_server_tests)
```

Если прилинковать `${CONAN_LIBS}`, в тесты может случайно попасть Boost.Test и появится ошибка:

```text
undefined reference to `test_main(int, char**)'
```

---

# 🧩 extra_data

В precode нет готового `extra_data`, поэтому он создаётся отдельно.

`extra_data` хранит JSON-массив `lootTypes` для каждой карты.

---

## src/extra_data.h

```cpp
#pragma once

#include "model.h"

#include <boost/json.hpp>

#include <unordered_map>

namespace extra_data {

namespace json = boost::json;

class ExtraData {
public:
    void AddLootTypes(const model::Map::Id& map_id, json::array loot_types) {
        loot_types_by_map_id_[map_id] = std::move(loot_types);
    }

    const json::array& GetLootTypes(const model::Map::Id& map_id) const {
        static const json::array empty;

        if (auto it = loot_types_by_map_id_.find(map_id); it != loot_types_by_map_id_.end()) {
            return it->second;
        }

        return empty;
    }

private:
    std::unordered_map<model::Map::Id, json::array, util::TaggedHasher<model::Map::Id>>
        loot_types_by_map_id_;
};

}  // namespace extra_data
```

---

## src/extra_data.cpp

```cpp
#include "extra_data.h"
```

---

# 🗺️ Изменения в model

## Map

В `Map` добавляется количество типов потерянных объектов:

```cpp
size_t GetLootTypesCount() const noexcept;
void SetLootTypesCount(size_t loot_types_count) noexcept;
```

private-поле:

```cpp
size_t loot_types_count_ = 0;
```

---

## LostObject

Перед `class GameSession` добавляется структура:

```cpp
struct LostObject {
    using Id = std::uint32_t;

    Id id = 0;
    std::uint32_t type = 0;
    Position position;
};
```

---

## GameSession

В `GameSession` добавляются:

```cpp
const std::unordered_map<LostObject::Id, LostObject>& GetLostObjects() const noexcept;

void GenerateLoot(std::chrono::milliseconds time_delta, loot_gen::LootGenerator& loot_generator);
```

private-поля:

```cpp
LostObject::Id next_lost_object_id_ = 0;
std::unordered_map<LostObject::Id, LostObject> lost_objects_;
```

---

## Game

В `Game` добавляется генератор:

```cpp
loot_gen::LootGenerator loot_generator_{std::chrono::milliseconds{1000}, 0.0};
```

и настройка:

```cpp
void SetLootGeneratorConfig(std::chrono::milliseconds period, double probability);
```

---

# 🔄 Game::Update

`Game::Update` теперь не только двигает собак, но и генерирует потерянные объекты:

```cpp
void Game::Update(double delta_seconds) {
    const auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(delta_seconds)
    );

    for (auto& session : sessions_) {
        session.Update(delta_seconds);
        session.GenerateLoot(delta_ms, loot_generator_);
    }
}
```

---

# 📥 json_loader

`LoadGame` теперь возвращает не только `model::Game`, но и `extra_data`.

```cpp
struct GameLoadResult {
    model::Game game;
    extra_data::ExtraData extra_data;
};
```

Ключевые действия:

```text
читает lootGeneratorConfig
настраивает Game::SetLootGeneratorConfig
читает lootTypes
сохраняет количество lootTypes в Map
сохраняет JSON lootTypes в ExtraData
```

---

## Важное исправление lootTypes

Если написать:

```cpp
extra_data.AddLootTypes(map.GetId(), json::array{loot_types});
```

получится лишний внешний массив:

```json
"lootTypes":[[{"name":"key"},{"name":"wallet"}]]
```

Правильно:

```cpp
extra_data.AddLootTypes(map.GetId(), loot_types);
```

Тогда будет:

```json
"lootTypes":[{"name":"key"},{"name":"wallet"}]
```

---

# 📤 json_serializer

`SerializeMap` теперь принимает `extra_data`:

```cpp
json::value SerializeMap(const model::Map& map, const extra_data::ExtraData& extra_data) {
    json::object obj;

    obj["id"] = *map.GetId();
    obj["name"] = map.GetName();
    obj["roads"] = SerializeRoads(map.GetRoads());
    obj["buildings"] = SerializeBuildings(map.GetBuildings());
    obj["offices"] = SerializeOffices(map.GetOffices());
    obj["lootTypes"] = extra_data.GetLootTypes(map.GetId());

    return obj;
}
```

---

# 🌐 request_handler

`RequestHandler` получает ссылку на `extra_data`:

```cpp
RequestHandler::RequestHandler(model::Game& game,
                               const extra_data::ExtraData& extra_data,
                               std::filesystem::path static_root,
                               bool manual_tick_enabled)
    : game_{game}
    , extra_data_{extra_data}
    , static_root_{std::move(static_root)}
    , manual_tick_enabled_{manual_tick_enabled} {
}
```

---

## `/api/v1/game/state`

В state добавляется поле:

```json
"lostObjects":{}
```

Когда объекты появились:

```json
"lostObjects":{
  "0":{"type":1,"pos":[10.0,0.0]}
}
```

---

# 🧪 tests/model-tests.cpp

Добавляются тесты на генерацию потерянных объектов:

```cpp
SCENARIO("Lost objects generation") {
    GIVEN("a game session with one road and one dog") {
        model::Map map{model::Map::Id{"map1"}, "Map 1"};
        map.AddRoad(model::Road{model::Road::HORIZONTAL, {0, 0}, 10});
        map.SetLootTypesCount(3);

        model::GameSession session{map, false};
        session.AddDog("dog");

        loot_gen::LootGenerator generator{1s, 1.0};

        WHEN("time passes") {
            session.GenerateLoot(1s, generator);

            THEN("lost object appears") {
                REQUIRE(session.GetLostObjects().size() == 1);

                const auto& object = session.GetLostObjects().begin()->second;

                CHECK(object.type < 3);
                CHECK(object.position.y == 0.0);
                CHECK(object.position.x >= 0.0);
                CHECK(object.position.x <= 10.0);
            }
        }
    }
}
```

---

# 🏗️ Сборка

```bash
cd ~/cppbackend/sprint3/problems/gen_objects/solution

rm -rf build
mkdir build
cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

# 🧪 Запуск тестов

Если бинарник лежит в `bin`:

```bash
./bin/game_server_tests
```

Если лежит прямо в `build`:

```bash
./game_server_tests
```

Через CTest:

```bash
ctest --output-on-failure
```

---

# 🚀 Запуск сервера

Если бинарник лежит прямо в `build`:

```bash
./game_server \
  --config-file ../data/config.json \
  --www-root ../static \
  --tick-period 100
```

Если в `bin`:

```bash
./bin/game_server \
  --config-file ../data/config.json \
  --www-root ../static \
  --tick-period 100
```

---

# 🔎 Проверка API

## Проверить карту

```bash
curl http://127.0.0.1:8080/api/v1/maps/map1
```

Должно быть:

```json
"lootTypes":[{"name":"key"},{"name":"wallet"}]
```

без двойных скобок.

---

## Подключиться к игре

```bash
curl -X POST http://127.0.0.1:8080/api/v1/game/join \
  -H "Content-Type: application/json" \
  -d '{"userName":"Dog","mapId":"map1"}'
```

Из ответа взять:

```text
authToken
```

---

## Проверить state

```bash
curl http://127.0.0.1:8080/api/v1/game/state \
  -H "Authorization: Bearer TOKEN"
```

Сначала может быть:

```json
"lostObjects":{}
```

Через несколько секунд:

```bash
curl http://127.0.0.1:8080/api/v1/game/state \
  -H "Authorization: Bearer TOKEN"
```

Появятся объекты:

```json
"lostObjects":{
  "0":{"type":1,"pos":[...]}
}
```

---

# 🧠 Главное в решении

```text
model хранит только количество lootTypes и игровые lostObjects
extra_data хранит JSON lootTypes для фронтенда
json_loader читает lootGeneratorConfig и lootTypes
Game::Update двигает собак и генерирует потерянные объекты
GET /api/v1/maps/{id} отдаёт lootTypes
GET /api/v1/game/state отдаёт lostObjects
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../../../../lessons/sprint_19_20/testing_and_debugging/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
