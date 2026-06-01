# 🎒 Find Return

## 📌 Описание

Задача развивает предыдущие задания:

```text
gen_objects
↓
gather-tests
↓
gather
↓
find_return
```

Теперь собака должна не только находить предметы, но и:

```text
подбирать предметы
хранить их в рюкзаке
учитывать вместимость рюкзака
возвращаться на базу
сдавать добычу
освобождать место для новых предметов
```

---

## 📂 Базовая подготовка

За основу берётся решение:

```text
gather/solution
```

Подготовка:

```bash
cd ~/cppbackend/sprint3/problems

rm -rf find_return/solution/*

cp -r gen_objects/solution/* find_return/solution/

cp gather/solution/src/collision_detector.h find_return/solution/src/
cp gather/solution/src/collision_detector.cpp find_return/solution/src/
cp gather/solution/src/geom.h find_return/solution/src/

cp -r find_return/precode/data find_return/solution/
cp -r find_return/precode/static find_return/solution/
```

---

## 🧠 Что изучается

| Тема                | Что реализуется                  |
| ------------------- | -------------------------------- |
| Lost Objects        | Сбор предметов                   |
| Bag Capacity        | Ограничение рюкзака              |
| Offices             | Базы для сдачи предметов         |
| Collision Detection | Коллизии с предметами и базами   |
| Game Events         | Обработка событий по времени     |
| Game Session        | Полное обновление состояния игры |

---

## 🎒 Рюкзак собаки

В класс `Dog` добавляется:

```cpp
struct CollectedObject {
    LostObject::Id id;
    std::uint32_t type;
};
```

Тип контейнера:

```cpp
using Bag = std::vector<CollectedObject>;
```

Поле:

```cpp
Bag bag_;
```

---

### Методы Dog

Получить содержимое:

```cpp
const Bag& GetBag() const noexcept;
```

Проверить заполнение:

```cpp
bool IsBagFull(size_t capacity) const noexcept;
```

Добавить предмет:

```cpp
void AddToBag(LostObject object);
```

Очистить рюкзак:

```cpp
void ClearBag() noexcept;
```

---

## 🗺 Вместимость карты

В класс `Map` добавляется:

```cpp
size_t bag_capacity_ = 3;
```

Методы:

```cpp
size_t GetBagCapacity() const noexcept;
```

```cpp
void SetBagCapacity(size_t capacity) noexcept;
```

---

## 📄 Чтение JSON-конфига

В `json_loader.cpp` читается:

```cpp
defaultBagCapacity
```

Пример:

```json
{
  "defaultBagCapacity": 3
}
```

Если параметр отсутствует:

```cpp
size_t default_bag_capacity = 3;
```

---

### Индивидуальная вместимость карты

Если карта содержит:

```json
{
  "bagCapacity": 5
}
```

то используется именно она.

Иначе применяется:

```text
defaultBagCapacity
```

---

## 🎯 Коллизии с предметами

После движения собак формируется:

```cpp
std::vector<DogMove>
```

Каждая запись содержит:

```cpp
Dog*
start position
finish position
```

---

### LootProvider

Создаётся адаптер:

```cpp
class LootProvider :
    public collision_detector::ItemGathererProvider
```

Он предоставляет:

```text
предметы
собак
```

для алгоритма:

```cpp
FindGatherEvents(...)
```

---

## 🏢 Коллизии с офисами

Создаётся аналогичный provider:

```cpp
OfficeProvider
```

Только вместо предметов используются:

```text
офисы карты
```

---

## 📐 Используемые радиусы

В collision detector поле:

```cpp
width
```

используется как радиус.

Поэтому:

```text
собака   = 0.3
предмет  = 0.0
офис     = 0.25
```

Получается из:

```text
0.6 / 2
0.0 / 2
0.5 / 2
```

---

## ⏱ Хронологический порядок

События должны обрабатываться строго по времени.

Создаётся структура:

```cpp
struct GameEvent {
    enum class Type {
        Loot,
        Office
    };

    Type type;
    double time;
    size_t dog_index;
    size_t object_index;
};
```

---

### Почему это важно

Например:

```text
взял предмет
↓
рюкзак полный
↓
сдал на базу
↓
взял следующий предмет
```

Если обработать события не по времени, логика будет неверной.

---

## 🔄 Обработка событий

Сначала собираются:

```text
loot events
office events
```

Потом объединяются:

```cpp
std::vector<GameEvent>
```

и сортируются:

```cpp
std::sort(...)
```

по:

```cpp
event.time
```

---

### Обработка предметов

Если событие:

```cpp
GameEvent::Type::Loot
```

то:

```cpp
dog.AddToBag(...)
```

и предмет помечается на удаление.

---

### Обработка офиса

Если событие:

```cpp
GameEvent::Type::Office
```

то:

```cpp
dog.ClearBag();
```

Все предметы считаются сданными.

---

## 🗑 Удаление предметов

После обработки:

```cpp
lost_objects_.erase(id);
```

Удаляются только реально собранные предметы.

---

## 🌐 API /game/state

В ответ игрока добавляется:

```json
{
  "bag": []
}
```

Пример:

```json
{
  "id": 0,
  "type": 2
}
```

---

## ⚙️ Изменения CMake

В оба target необходимо добавить:

```cmake
src/collision_detector.h
src/collision_detector.cpp
src/geom.h
```

---

## ⚠️ Важность geom.h

Файл:

```text
src/geom.h
```

обязателен.

Используется в:

```cpp
collision_detector.h
```

через:

```cpp
#include "geom.h"
```

и содержит:

```cpp
geom::Point2D
```

---

## 🛠 Проверка сборки

```bash
cd ~/cppbackend/sprint3/problems/find_return/solution

rm -rf build
mkdir build
cd build

source ~/conan-venv/bin/activate

conan install .. \
  --build=missing \
  -s build_type=Release \
  -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .

ctest --output-on-failure
```

---

## ▶️ Проверка сервера

Запуск:

```bash
./bin/game_server \
  --config-file ../data/config.json \
  --www-root ../static \
  --tick-period 100
```

Подключение игрока:

```bash
TOKEN=$(curl -s -X POST \
http://127.0.0.1:8080/api/v1/game/join \
-H "Content-Type: application/json" \
-d '{"userName":"Dog","mapId":"map1"}' \
| jq -r '.authToken')
```

Проверка состояния:

```bash
curl -s \
http://127.0.0.1:8080/api/v1/game/state \
-H "Authorization: Bearer $TOKEN" | jq
```

Ожидается:

```json
"bag": []
```

---

## 🚀 Git

```bash
cd ~/cppbackend

git add sprint3/problems/find_return/solution

git commit -m "implement loot collection and return"

git push origin final-task-pr
```

---

## 🏁 Итог

Главная цепочка задания:

```text
Lost Object
↓
Collision Detector
↓
FindGatherEvents
↓
Dog Bag
↓
Bag Capacity
↓
Office
↓
ClearBag
↓
Game State
```

Главная идея:

```text
Собака должна собирать предметы, учитывать вместимость рюкзака, возвращаться на базу и сдавать добычу, а все игровые события должны обрабатываться строго в хронологическом порядке.
```

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../../../../lessons/sprint_19_20/collision_detection_and_statistics/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
