# 🏆 Scores

## 📌 Описание

Задача развивает механику из:

```text
gen_objects
↓
gather
↓
find_return
↓
scores
```

Теперь собранные предметы должны приносить игроку очки.

Полный игровой цикл:

```text
Предмет
↓
Собака подбирает предмет
↓
Предмет попадает в bag
↓
Собака достигает офиса
↓
Предметы сдаются
↓
Начисляется score
↓
Счёт отображается в game/state
```

---

## 📂 Подготовка решения

За основу берётся полностью рабочее решение:

```text
find_return/solution
```

Подготовка:

```bash
cd ~/cppbackend/sprint3/problems

rm -rf scores/solution/*

cp -r find_return/solution/* scores/solution/

cp -r scores/precode/data scores/solution/
cp -r scores/precode/static scores/solution/

cd scores/solution
```

---

## 🧠 Что изучается

| Тема        | Что реализуется           |
| ----------- | ------------------------- |
| Loot Values | Стоимость предметов       |
| Score       | Система очков игрока      |
| Offices     | Сдача предметов           |
| JSON Config | Чтение value из lootTypes |
| Game State  | Отображение score         |
| Game Logic  | Начисление награды        |

---

## 💰 Стоимость предметов

Каждый тип предмета теперь содержит:

```json
{
    "value": 10
}
```

Пример:

```json
{
    "name": "key",
    "value": 10
}
```

Стоимость хранится внутри:

```text
lootTypes
```

---

## 📦 ExtraData

В `src/extra_data.h` добавляется метод:

```cpp
int GetLootValue(
    const model::Map::Id& map_id,
    std::uint32_t type
) const;
```

Он позволяет получить стоимость предмета по:

```text
map_id
+
type
```

---

## 🗺 Хранение стоимости в Map

В класс `Map` добавляется:

```cpp
std::vector<int> loot_type_values_;
```

Методы:

```cpp
void SetLootTypeValues(std::vector<int> values);
```

```cpp
const std::vector<int>& GetLootTypeValues() const noexcept;
```

Таким образом карта знает стоимость каждого типа предмета.

---

## 🐕 Очки игрока

В класс `Dog` добавляется поле:

```cpp
int score_ = 0;
```

Методы:

```cpp
int GetScore() const noexcept;
```

```cpp
void AddScore(int score) noexcept;
```

---

## 🎒 Сдача рюкзака

Добавляется метод:

```cpp
std::vector<CollectedObject> DropBag();
```

Он:

```text
возвращает содержимое bag
очищает bag
```

Это удобно при сдаче предметов на базу.

---

## 🏢 ReturnDogLootToOffice

В `GameSession` реализуется:

```cpp
void ReturnDogLootToOffice(Dog& dog);
```

Логика:

```text
получить содержимое рюкзака
↓
посчитать стоимость предметов
↓
начислить score
↓
очистить рюкзак
```

---

### Подсчёт стоимости

Используются:

```cpp
dog.DropBag();
```

и

```cpp
map_.GetLootTypeValues();
```

Для каждого предмета:

```cpp
score += values.at(object.type);
```

После чего:

```cpp
dog.AddScore(score);
```

---

## 🔄 Изменение обработки офисов

В предыдущем задании использовалось:

```cpp
dog.ClearBag();
```

Теперь заменяется на:

```cpp
ReturnDogLootToOffice(dog);
```

Таким образом предметы не исчезают бесплатно, а превращаются в очки.

---

## 📄 Чтение value из JSON

В `json_loader.cpp` после чтения:

```cpp
lootTypes
```

формируется:

```cpp
std::vector<int> loot_type_values;
```

Пример:

```json
{
    "value": 10
}
```

превращается в:

```cpp
loot_type_values.push_back(10);
```

После чего:

```cpp
map.SetLootTypeValues(
    std::move(loot_type_values)
);
```

---

## 🌐 API /game/state

В ответе игрока появляется новое поле:

```json
{
    "score": 0
}
```

Пример полного объекта:

```json
{
    "pos": [0, 0],
    "speed": [0, 0],
    "dir": "U",
    "bag": [],
    "score": 0
}
```

После сдачи предметов значение увеличивается.

---

## ⚙️ Сборка

```bash
cd ~/cppbackend/sprint3/problems/scores/solution

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

Подключение:

```bash
TOKEN=$(curl -s -X POST \
http://127.0.0.1:8080/api/v1/game/join \
-H "Content-Type: application/json" \
-d '{"userName":"Dog","mapId":"map1"}' \
| jq -r '.authToken')
```

Проверка:

```bash
curl -s \
http://127.0.0.1:8080/api/v1/game/state \
-H "Authorization: Bearer $TOKEN" | jq
```

Ожидается:

```json
"score": 0
```

После:

```text
подобрать предмет
↓
дойти до офиса
```

значение должно увеличиться.

---

## 🚀 Git

```bash
cd ~/cppbackend

git add sprint3/problems/scores/solution

git commit -m "add scores for returned loot"

git push origin final-task-pr
```

---

## 🏁 Итог

Главная цепочка задания:

```text
Loot Type
↓
value
↓
Dog Bag
↓
Office
↓
ReturnDogLootToOffice
↓
Score
↓
Game State
```

Главная идея:

```text
Собранные предметы приобретают ценность.
После сдачи на базу их стоимость превращается в очки игрока, которые сохраняются внутри Dog и отображаются через API.
```

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../../../../lessons/sprint_19_20/collision_detection_and_statistics/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
