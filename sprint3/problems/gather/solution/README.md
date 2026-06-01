# 🎯 Gather

## 📌 Описание

Задача посвящена реализации функции:

```cpp
std::vector<GatheringEvent> FindGatherEvents(
    const ItemGathererProvider& provider
);
```

Функция должна находить столкновения между:

```text
Gatherer (сборщик)
Item (предмет)
```

и возвращать события сбора в хронологическом порядке.

---

## 📂 Структура решения

```text
solution/
├── Dockerfile
├── README.md
├── src/
│   ├── collision_detector.cpp
│   ├── collision_detector.h
│   └── geom.h
└── tests/
    └── collision-detector-tests.cpp
```

---

## 🧠 Что изучается

| Тема                 | Что реализуется                    |
| -------------------- | ---------------------------------- |
| Геометрия            | Расстояние от точки до траектории  |
| Проекция             | Поиск положения объекта на отрезке |
| Коллизии             | Проверка пересечения радиусов      |
| Сортировка           | Хронологический порядок событий    |
| ItemGathererProvider | Абстракция источника данных        |
| Catch2               | Проверка реализации тестами        |

---

## 🔧 Подготовка решения

За основу берётся решение из предыдущей задачи:

```text
gather-tests/solution
```

Копирование:

```bash
cd ~/cppbackend/sprint3/problems

rm -rf gather/solution

cp -r gather-tests/solution gather/solution
```

После этого реализуется только:

```cpp
FindGatherEvents(...)
```

---

## ⚙️ Алгоритм работы

Функция выполняет несколько шагов.

### Шаг 1. Перебор всех сборщиков

```cpp
for (size_t gatherer_id = 0;
     gatherer_id < provider.GatherersCount();
     ++gatherer_id)
```

Получаем каждого сборщика:

```cpp
const Gatherer gatherer =
    provider.GetGatherer(gatherer_id);
```

---

### Шаг 2. Пропуск неподвижных объектов

Если сборщик не двигался:

```cpp
start_pos == end_pos
```

то столкновения не учитываются:

```cpp
continue;
```

По условию задачи неподвижный объект ничего не собирает.

---

### Шаг 3. Перебор всех предметов

Для каждого сборщика:

```cpp
for (size_t item_id = 0;
     item_id < provider.ItemsCount();
     ++item_id)
```

берётся предмет:

```cpp
const Item item =
    provider.GetItem(item_id);
```

---

### Шаг 4. Вычисление расстояния до траектории

Используется:

```cpp
TryCollectPoint(...)
```

которая возвращает:

```cpp
CollectionResult
```

с полями:

```cpp
sq_distance
proj_ratio
```

---

### Шаг 5. Проверка радиуса сбора

Радиус столкновения:

```cpp
const double collect_radius =
    gatherer.width + item.width;
```

Если:

```text
расстояние ≤ W + w
```

то создаётся событие:

```cpp
GatheringEvent
```

---

### Шаг 6. Добавление события

Создаётся запись:

```cpp
events.push_back(...)
```

с сохранением:

```cpp
item_id
gatherer_id
sq_distance
time
```

---

### Шаг 7. Сортировка

После завершения перебора:

```cpp
std::sort(...)
```

Сортировка идёт по:

```cpp
event.time
```

От ранних столкновений к поздним.

---

## 📐 Геометрическая модель

Используются три точки:

```text
A — начало движения
B — конец движения
C — предмет
```

Схема:

```text
A ---------------- B

        |
        |
        C
```

Вычисляется:

```text
проекция точки C на отрезок AB
```

и расстояние:

```text
от C до траектории
```

---

## 🧪 Проверка решения

Используются тесты из предыдущей задачи:

```text
gather-tests
```

Проверяются:

```text
обычные столкновения
граничные случаи
неподвижные сборщики
хронологический порядок
несколько объектов
отсутствие ложных событий
```

---

## 🛠️ Сборка

```bash
cd ~/cppbackend/sprint3/problems/gather/solution

rm -rf build
mkdir build
cd build

source ~/conan-venv/bin/activate

conan install .. \
    --build=missing \
    -s compiler.libcxx=libstdc++11 \
    -s build_type=Release

cmake -DCMAKE_BUILD_TYPE=Release ..

cmake --build .
```

---

## ▶️ Запуск тестов

Локально:

```bash
./collision_detection_tests
```

Через Docker:

```bash
sudo docker build -t gather_solution .

sudo docker run --rm gather_solution
```

---

## ⚠️ Важность geom.h

Файл:

```text
src/geom.h
```

обязателен.

Он используется внутри:

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

который применяется в структурах:

```cpp
Item
Gatherer
```

Без него проект не соберётся.

---

## 🏁 Итог

Главная цепочка задачи:

```text
ItemGathererProvider
↓
Gatherer
↓
Item
↓
TryCollectPoint
↓
CollectionResult
↓
FindGatherEvents
↓
GatheringEvent
↓
Сортировка по времени
```

Главная идея:

```text
Нужно реализовать алгоритм поиска столкновений между движущимися сборщиками и предметами, корректно вычислить расстояния, проверить радиус сбора и вернуть события в правильном хронологическом порядке.
```

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../../../../lessons/sprint_19_20/collision_detection_and_statistics/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
