# ✅ Lesson 3/9 — Google Test

## 📌 Описание

В этом уроке изучается фреймворк модульного тестирования **Google Test**.

Проект собирается через:

```text
Conan
↓
CMake
↓
Google Test
↓
CTest
```

В качестве примера используется та же бизнес-логика, что и в уроке про Boost.Test:

```cpp
bool IsLeapYear(int year);
```

---

## 🧠 Что изучается

| 📌 Тема      | 📖 Что изучается                          | ✅ Итог                           |
| ------------ | ----------------------------------------- | -------------------------------- |
| Google Test  | Фреймворк тестирования C++ кода           | Написание юнит-тестов            |
| Conan        | Подключение зависимости `gtest/1.10.0`    | Установка Google Test            |
| CMake        | Создание приложения и тестового бинарника | Сборка проекта                   |
| CTest        | Запуск тестов из CMake                    | Автоматическая проверка          |
| ASSERT_TRUE  | Проверка истинного условия                | Тест падает при ошибке           |
| ASSERT_FALSE | Проверка ложного условия                  | Проверка отрицательных случаев   |
| ABI          | Совместимость стандартной библиотеки C++  | Исправление ошибки `[abi:cxx11]` |

---

## 📂 Структура проекта

```text
lesson_3_9_google_test/
├── CMakeLists.txt
├── conanfile.txt
├── README.md
│
├── src/
│   ├── main.cpp
│   ├── leap_year.h
│   └── leap_year.cpp
│
└── tests/
    └── google_test_main.cpp
```

---

## 📦 conanfile.txt

Файл:

```text
conanfile.txt
```

Содержимое:

```ini
[requires]
gtest/1.10.0

[generators]
cmake_multi
```

---

## 🧩 CMakeLists.txt

Рабочий вариант:

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

add_executable(google_test_example
    tests/google_test_main.cpp
    src/leap_year.h
    src/leap_year.cpp
)

target_link_libraries(google_test_example PRIVATE CONAN_PKG::gtest)

add_test(NAME google_test_example COMMAND google_test_example)
```

---

## 🧪 tests/google_test_main.cpp

Рабочий тест:

```cpp
#include <gtest/gtest.h>

#include "../src/leap_year.h"

TEST(LeapYearTestSuite, LeapYearIsDivisibleBy4) {
    ASSERT_TRUE(IsLeapYear(2020));
    ASSERT_FALSE(IsLeapYear(2021));
    ASSERT_FALSE(IsLeapYear(2022));
    ASSERT_FALSE(IsLeapYear(2023));
    ASSERT_TRUE(IsLeapYear(2024));
}

TEST(LeapYearTestSuite, LeapYearIsNotDivisibleBy100ButIsDivisibleBy400) {
    ASSERT_FALSE(IsLeapYear(1900));
    ASSERT_TRUE(IsLeapYear(2000));
}
```

---

## ⚙️ Сборка

Перейти в папку урока:

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_3_9_google_test
```

Активировать Conan:

```bash
source ~/conan-venv/bin/activate
```

Полностью пересобрать:

```bash
rm -rf build

mkdir build

cd build

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск тестов

Запуск напрямую:

```bash
./google_test_example
```

Или через CTest:

```bash
ctest --output-on-failure
```

Ожидаемый результат:

```text
[  PASSED  ] 2 tests.
100% tests passed, 0 tests failed out of 1
```

---

## ⚠️ Важная ошибка `[abi:cxx11]`

Если при сборке появляются ошибки вида:

```text
undefined reference ... [abi:cxx11]
```

это означает несовпадение ABI стандартной библиотеки C++.

Проблема была в том, что `gtest` был установлен Conan-ом с ABI:

```text
compiler.libcxx=libstdc++
```

а проект компилировался с новым ABI:

```text
libstdc++11
```

---

## ✅ Правильное исправление

Переустановить зависимость с правильным ABI:

```bash
conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11
```

Ключевая часть:

```bash
-s compiler.libcxx=libstdc++11
```

После этого обычная линковка работает корректно:

```cmake
target_link_libraries(google_test_example PRIVATE CONAN_PKG::gtest)
```

---

## 🧠 Главное отличие от Boost.Test

| Boost.Test                                     | Google Test                                |
| ---------------------------------------------- | ------------------------------------------ |
| Можно использовать header-only режим           | Обычно подключается как библиотека         |
| `BOOST_AUTO_TEST_CASE`                         | `TEST(TestSuite, TestName)`                |
| `BOOST_CHECK`                                  | `ASSERT_TRUE`, `ASSERT_FALSE`, `EXPECT_EQ` |
| Может требовать отдельную линковку Boost.Test  | Требует корректную линковку `gtest`        |
| Ошибка решалась через `included/unit_test.hpp` | Ошибка решалась через ABI `libstdc++11`    |

---

## 🏁 Итог

Главная цепочка урока:

```text
conanfile.txt
↓
gtest/1.10.0
↓
conan install с libstdc++11
↓
CMake
↓
target_link_libraries(... CONAN_PKG::gtest)
↓
google_test_example
↓
ctest
```

Главная идея:

```text
Google Test удобно подключается через Conan и CMake, но важно следить, чтобы ABI зависимостей совпадал с ABI проекта.
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
