# 🧪 Lesson 4/9 — Catch2

## 📌 Описание

Мини-упражнение показывает полный рабочий цикл подключения `Catch2` к C++ проекту через `Conan` и `CMake`.

В проекте есть обычное приложение `application` и отдельный тестовый исполняемый файл `catch_test_example`.

Основная функция для тестирования — `IsLeapYear`.

---

## 🧠 Что изучается

| 📌 Тема     | 📖 Что происходит                                         | ✅ Итог                                     |
| ----------- | --------------------------------------------------------- | ------------------------------------------ |
| Catch2      | Используется как C++ test framework                       | Можно писать unit-тесты                    |
| Conan       | Подтягивает зависимость `catch2/3.1.0`                    | Зависимость не нужно хранить в репозитории |
| CMake       | Собирает приложение и тесты                               | Проект имеет понятную структуру сборки     |
| CTest       | Запускает тесты через `ctest`                             | Тесты можно запускать стандартным способом |
| `TEST_CASE` | Создаёт отдельный тестовый сценарий                       | Тесты становятся читаемыми                 |
| `CHECK`     | Проверяет условие и продолжает тест                       | Удобно для нескольких проверок             |
| `REQUIRE`   | Проверяет условие и останавливает текущий тест при ошибке | Удобно для критичных проверок              |

---

## 📂 Структура упражнения

```text
lesson_4_9_catch2/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── main.cpp
│   ├── leap_year.h
│   └── leap_year.cpp
└── tests/
    └── catch_test_main.cpp
```

---

## 🛠️ Создание структуры

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging

mkdir -p lesson_4_9_catch2

cd lesson_4_9_catch2

mkdir -p src tests

touch src/main.cpp src/leap_year.h src/leap_year.cpp
touch tests/catch_test_main.cpp
touch conanfile.txt CMakeLists.txt
```

---

## 📦 conanfile.txt

Открыть:

```bash
nano conanfile.txt
```

Вставить:

```ini
[requires]
catch2/3.1.0

[generators]
cmake_multi
```

---

## 🧩 src/leap_year.h

Открыть:

```bash
nano src/leap_year.h
```

Вставить:

```cpp
#pragma once

bool IsLeapYear(int year);
```

---

## 🧩 src/leap_year.cpp

Открыть:

```bash
nano src/leap_year.cpp
```

Вставить:

```cpp
#include "leap_year.h"

bool IsLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}
```

---

## 🧩 src/main.cpp

Открыть:

```bash
nano src/main.cpp
```

Вставить:

```cpp
#include <iostream>

#include "leap_year.h"

int main() {
    using namespace std::literals;

    std::cout << "Enter year: "sv;

    int year;
    std::cin >> year;

    std::cout << year << " is a "sv
              << (IsLeapYear(year) ? "leap"sv : "non-leap"sv)
              << " year"sv
              << std::endl;
}
```

---

## 🧪 tests/catch_test_main.cpp

Открыть:

```bash
nano tests/catch_test_main.cpp
```

Вставить:

```cpp
#include <catch2/catch_test_macros.hpp>

#include "../src/leap_year.h"

TEST_CASE("Leap year testing") {
    CHECK(IsLeapYear(2020));
    CHECK(!IsLeapYear(2021));
    CHECK(!IsLeapYear(2022));
    CHECK(!IsLeapYear(2023));
    CHECK(IsLeapYear(2024));
    CHECK(!IsLeapYear(1900));
    CHECK(IsLeapYear(2000));
}

int Sqr(int x) {
    return x * x;
}

TEST_CASE("Sqr tests") {
    CHECK(Sqr(3) == 9);
    CHECK(Sqr(2) == 4);
    REQUIRE(Sqr(4) == Sqr(-4));
}

TEST_CASE("Complex expressions") {
    CHECK(((2 < 3) && (3 > 2)));
}
```

---

## ⚙️ CMakeLists.txt

Открыть:

```bash
nano CMakeLists.txt
```

Вставить:

```cmake
enable_testing()

cmake_minimum_required(VERSION 3.11)

project(samples CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(application
    src/main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

add_executable(catch_test_example
    tests/catch_test_main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

target_link_libraries(catch_test_example PRIVATE CONAN_PKG::catch2)

add_test(NAME catch_test_example COMMAND catch_test_example)
```

---

## 💾 Сохранение в nano

Для каждого файла:

```text
Ctrl + O
Enter
Ctrl + X
```

---

## 🏗️ Сборка

Перейти в папку урока:

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_4_9_catch2
```

Очистить старую сборку:

```bash
rm -rf build
mkdir build
cd build
```

Активировать Conan-окружение:

```bash
source ~/conan-venv/bin/activate
```

Установить зависимости с правильным ABI:

```bash
conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11
```

Сконфигурировать CMake:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Собрать проект:

```bash
cmake --build .
```

---

## ▶️ Запуск приложения

```bash
./bin/application
```

или, если бинарник лежит прямо в `build`:

```bash
./application
```

---

## 🧪 Запуск тестов

Запустить тестовый бинарник напрямую:

```bash
./bin/catch_test_example
```

или:

```bash
./catch_test_example
```

Запустить через CTest:

```bash
ctest --output-on-failure
```

---

## 📌 Главное по теории

`Catch2` — фреймворк для тестирования C++.

```cpp
TEST_CASE("Leap year testing")
```

создаёт тестовый сценарий.

```cpp
CHECK(IsLeapYear(2020));
```

проверяет условие, но продолжает выполнение теста дальше, если проверка упала.

```cpp
REQUIRE(Sqr(4) == Sqr(-4));
```

проверяет условие и останавливает текущий `TEST_CASE`, если проверка упала.

Разница:

```text
CHECK   — ошибка не фатальная
REQUIRE — ошибка фатальная для текущего TEST_CASE
```

Сложные выражения в Catch2 лучше писать с двойными скобками:

```cpp
CHECK(((2 < 3) && (3 > 2)));
```

---

## 🏁 Итог

В этом упражнении собран минимальный, но полноценный C++ проект:

```text
application
```

для обычного запуска программы

и

```text
catch_test_example
```

для запуска unit-тестов.

Главная цепочка:

```text
Conan
↓
Catch2
↓
CMake
↓
CTest
↓
unit-тесты
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
