# 💾 Serialization — Lesson 5/7

## 📌 Описание

В этом уроке разбирается сериализация объектов C++ с помощью библиотеки Boost.Serialization.

Сериализация — это процесс преобразования объекта программы в формат, пригодный для хранения или передачи.

Например:

```text
объект
↓
архив
↓
файл
```

или:

```text
объект
↓
архив
↓
сеть
```

Обратный процесс называется десериализацией.

---

## 🧠 Что изучается

| Тема              | Что изучается                               |
| ----------------- | ------------------------------------------- |
| text_oarchive     | Текстовая сериализация                      |
| text_iarchive     | Текстовая десериализация                    |
| std::stringstream | Хранение сериализованных данных в памяти    |
| std::vector       | Сериализация контейнеров                    |
| SerDog            | Сериализация игровых объектов               |
| Code Smells       | Разделение предметной модели и сериализации |
| Conan             | Управление зависимостями                    |
| CMake             | Сборка практических проектов                |

---

## 📂 Практики урока

| Практика              | Что изучается                       |
| --------------------- | ----------------------------------- |
| `01_text_archives`    | text_oarchive и text_iarchive       |
| `02_supported_types`  | Сериализация std::vector            |
| `03_ser_dog_finalize` | Финальная структура SerDog          |
| `04_code_smells`      | Архитектурные проблемы сериализации |

---

## ⚙️ Общая схема сборки

Каждая практика содержит:

```text
main.cpp
CMakeLists.txt
conanfile.txt
```

Сборка:

```bash
rm -rf build
mkdir build
cd build

conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

cmake --build .

./<имя_исполняемого_файла>
```

---

## 📚 Практика 1 — text_oarchive и text_iarchive

Папка:

```text
01_text_archives
```

Изучается работа:

```text
text_oarchive
text_iarchive
```

Сериализация объекта:

```text
Box
↓
stringstream
↓
восстановление объекта
```

---

## 📚 Практика 2 — Supported Types

Папка:

```text
02_supported_types
```

Изучается сериализация:

```cpp
std::vector<Item>
```

Главное правило:

```text
контейнер сериализуем,
если сериализуемы его элементы
```

---

## 📚 Практика 3 — SerDog

Папка:

```text
03_ser_dog_finalize
```

Изучается структура:

```cpp
SerDog
```

Содержит:

```text
id
direction
pos_x
pos_y
speed_x
speed_y
name
```

Используется как переносчик данных между игровой логикой и сериализацией.

---

## 📚 Практика 4 — Code Smells

Папка:

```text
04_code_smells
```

Изучается принцип:

```text
Single Responsibility Principle
```

Правильная схема:

```text
Dog
↓
ToSerDog()
↓
SerDog
↓
Serialization
```

Игровой объект не должен зависеть от Boost.Serialization напрямую.

---

## 🔨 Сборка всех практик

Из папки урока:

```bash
for dir in 01_text_archives 02_supported_types 03_ser_dog_finalize 04_code_smells; do
    echo "Building $dir"
    cd "$dir"

    rm -rf build
    mkdir build
    cd build

    conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

    cmake --build .

    cd ../..
done
```

---

## 🏁 Итог

Главная цепочка урока:

```text
Object
↓
Serialization
↓
Archive
↓
Storage
↓
Deserialization
↓
Object
```

Главная идея:

```text
Сериализация позволяет сохранять и восстанавливать состояние игровых объектов, а использование SerDog помогает не связывать игровую модель напрямую с библиотекой сериализации.
```

---

## ⬅️ Назад

[Вернуться к Serialization](../README.md)

[Вернуться в Sprint 4](../../../../README.md)
