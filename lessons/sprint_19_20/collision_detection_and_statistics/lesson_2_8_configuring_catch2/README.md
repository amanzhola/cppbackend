# 🧪 Lesson 2/8 — Configuring Catch2

## 📌 Описание

Это упражнение показывает правильную организацию C++ проекта с использованием:

* Conan
* CMake
* Catch2
* CTest
* статической библиотеки

Главная идея урока:

```text
общий код
↓
STATIC библиотека
↓
используется приложением
и тестами
```

Вместо двойной компиляции одинаковых `.cpp` файлов общий код собирается один раз и затем линкуется ко всем исполняемым файлам.

---

## 🧠 Что изучается

| 📌 Тема              | 📖 Что происходит                   | ✅ Итог                     |
| -------------------- | ----------------------------------- | -------------------------- |
| Catch2               | Используется как тестовый фреймворк | Можно писать unit-тесты    |
| Conan                | Устанавливает Catch2                | Управление зависимостями   |
| Static Library       | Общий код собирается один раз       | Нет дублирования сборки    |
| CTest                | Автоматически находит тесты Catch2  | Удобный запуск тестов      |
| catch_discover_tests | Интеграция Catch2 и CTest           | Каждый тест виден отдельно |

---

## 📂 Структура проекта

```text
lesson_2_8_configuring_catch2/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── math_tools.h
│   ├── math_tools.cpp
│   └── main.cpp
└── tests/
    └── math_tools_tests.cpp
```

---

## 📦 conanfile.txt

```ini
[requires]
catch2/3.1.0

[generators]
cmake
```

---

## 🧩 math_tools.h

```cpp
#pragma once

int Sqr(int value);

bool IsEven(int value);
```

---

## 🧩 math_tools.cpp

```cpp
#include "math_tools.h"

int Sqr(int value) {
    return value * value;
}

bool IsEven(int value) {
    return value % 2 == 0;
}
```

---

## 🧩 main.cpp

```cpp
#include "math_tools.h"

#include <iostream>

int main() {
    std::cout << "Sqr(5) = " << Sqr(5) << std::endl;
    std::cout << "IsEven(10) = "
              << std::boolalpha
              << IsEven(10)
              << std::endl;

    return 0;
}
```

---

## 🧪 math_tools_tests.cpp

```cpp
#include <catch2/catch_test_macros.hpp>

#include "../src/math_tools.h"

TEST_CASE("Sqr calculates square", "[math]") {
    CHECK(Sqr(3) == 9);
    CHECK(Sqr(-5) == 25);
    CHECK(Sqr(0) == 0);
}

TEST_CASE("IsEven checks even numbers", "[math]") {
    CHECK(IsEven(2));
    CHECK(IsEven(0));
    CHECK_FALSE(IsEven(3));
    CHECK_FALSE(IsEven(-7));
}
```

---

## ⚙️ CMakeLists.txt

Главная идея урока — общий код выносится в отдельную библиотеку:

```cmake
add_library(math_tools_lib STATIC
    src/math_tools.cpp
)
```

Затем приложение:

```cmake
target_link_libraries(math_app PRIVATE
    math_tools_lib
)
```

И тесты:

```cmake
target_link_libraries(math_tests PRIVATE
    math_tools_lib
    ${CONAN_LIBS}
)
```

Используют одну и ту же библиотеку.

---

## 🏗️ Сборка

```bash
rm -rf build

mkdir build

cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск приложения

```bash
./bin/math_app
```

Ожидаемо:

```text
Sqr(5) = 25
IsEven(10) = true
```

---

## 🧪 Запуск тестов

Все тесты:

```bash
./bin/math_tests
```

Ожидаемо:

```text
All tests passed
```

---

## 📋 Список тестов

```bash
./bin/math_tests --list-tests
```

Ожидаемо:

```text
Sqr calculates square
IsEven checks even numbers
```

---

## 🎯 Запуск одного теста

```bash
./bin/math_tests "Sqr calculates square"
```

---

## 🏷️ Запуск по тегу

```bash
./bin/math_tests "[math]"
```

---

## 🔍 Интеграция с CTest

Ключевая строка:

```cmake
catch_discover_tests(math_tests)
```

Благодаря ей:

```bash
ctest
```

видит отдельные Catch2 тесты, а не просто бинарник.

Запуск:

```bash
ctest
```

---

## Было плохо

```cmake
add_executable(MyProgram model.cpp use_cases.cpp main.cpp)

add_executable(MyTest model.cpp use_cases.cpp test.cpp)
```

Одинаковые файлы компилируются дважды.

---

## Стало лучше

```cmake
add_library(MyLib STATIC
    model.cpp
    use_cases.cpp
)

add_executable(MyProgram main.cpp)

add_executable(MyTest test.cpp)

target_link_libraries(MyProgram MyLib)

target_link_libraries(MyTest MyLib)
```

---

## 🏁 Итог

Главная идея урока:

```text
общий код
↓
STATIC библиотека
↓
программа
+
тесты
```

Преимущества:

```text
меньше дублирования
быстрее сборка
чище CMakeLists.txt
проще сопровождение проекта
```

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
