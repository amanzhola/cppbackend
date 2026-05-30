# 🔴🟢 Lesson 5/9 — TDD

## 📌 Описание

Мини-упражнение показывает первую итерацию TDD на простом классе `TV`.

TDD означает Test Driven Development — разработка через тестирование.

Главная идея:

```text
сначала пишем тест
↓
убеждаемся, что он падает
↓
пишем минимальный код
↓
убеждаемся, что тест проходит
```

В этом упражнении проверяется правило:

```text
новый телевизор должен быть выключен по умолчанию
```

---

## 🧠 Что изучается

| 📌 Тема    | 📖 Что происходит                                                    | ✅ Итог                                                    |
| ---------- | -------------------------------------------------------------------- | --------------------------------------------------------- |
| TDD        | Сначала пишется тест, потом реализация                               | Код развивается через проверяемые требования              |
| RED        | Тест падает на неправильной заглушке                                 | Видно, что тест действительно проверяет поведение         |
| GREEN      | Код исправляется минимально                                          | Тест становится зелёным                                   |
| Boost.Test | Используется header-only вариант `boost/test/included/unit_test.hpp` | Можно писать unit-тесты без отдельной линковки Boost.Test |
| Conan      | Подтягивает `boost/1.78.0`                                           | Зависимость управляется отдельно от кода                  |
| CMake      | Собирает тестовый бинарник `tv_tests`                                | Проект можно стабильно пересобирать                       |
| CTest      | Запускает тесты через стандартный механизм CMake                     | Тесты можно запускать командой `ctest`                    |

---

## 📂 Структура упражнения

```text
lesson_5_9_tdd/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── tv.h
│   └── tv.cpp
└── tests/
    └── tv_tests.cpp
```

---

## 🛠️ Создание папки урока

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging

mkdir -p lesson_5_9_tdd

cd lesson_5_9_tdd
```

---

## 🧱 Создание файлов

```bash
mkdir -p src tests

touch src/tv.h
touch src/tv.cpp
touch tests/tv_tests.cpp
touch conanfile.txt
touch CMakeLists.txt
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
boost/1.78.0

[generators]
cmake_multi
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

project(tdd_tv_example CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(tv_tests
    tests/tv_tests.cpp
    src/tv.h
    src/tv.cpp
)

add_test(NAME tv_tests COMMAND tv_tests)
```

Здесь используется Boost.Test через header-only файл:

```cpp
#include <boost/test/included/unit_test.hpp>
```

Поэтому отдельная строка:

```cmake
target_link_libraries(... CONAN_PKG::boost)
```

не нужна.

---

# 🔴 RED стадия — тест должен упасть

## 🧩 src/tv.h

Открыть:

```bash
nano src/tv.h
```

Вставить неправильную заглушку:

```cpp
#pragma once

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return true;
    }
};
```

---

## 🧩 src/tv.cpp

Открыть:

```bash
nano src/tv.cpp
```

Вставить:

```cpp
#include "tv.h"
```

---

## 🧪 tests/tv_tests.cpp

Открыть:

```bash
nano tests/tv_tests.cpp
```

Вставить:

```cpp
#define BOOST_TEST_MODULE TV tests
#include <boost/test/included/unit_test.hpp>

#include "../src/tv.h"

BOOST_AUTO_TEST_SUITE(TV_)

BOOST_AUTO_TEST_CASE(is_off_by_default) {
    TV tv;

    BOOST_CHECK(!tv.IsTurnedOn());
}

BOOST_AUTO_TEST_SUITE_END()
```

---

## 🏗️ Собрать RED

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_5_9_tdd

rm -rf build
mkdir build
cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запустить RED

```bash
./tv_tests
```

Ожидаемо тест должен упасть:

```text
check !tv.IsTurnedOn() has failed
```

Это правильно. Это стадия `RED`.

---

# 🟢 GREEN стадия — исправляем минимально

## 🧩 Исправить src/tv.h

Вернуться в папку урока:

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_5_9_tdd
```

Открыть:

```bash
nano src/tv.h
```

Заменить весь файл на:

```cpp
#pragma once

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

private:
    bool is_turned_on_ = false;
};
```

---

## 🏗️ Пересобрать GREEN

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_5_9_tdd/build

cmake --build .
```

---

## ▶️ Запустить GREEN

```bash
./tv_tests
```

Ожидаемо:

```text
Running 1 test case...

*** No errors detected
```

Через CTest:

```bash
ctest --output-on-failure
```

Ожидаемо:

```text
100% tests passed
```

---

## 🧠 Смысл RED → GREEN

RED:

```cpp
bool IsTurnedOn() const noexcept {
    return true;
}
```

Тест падает, потому что новый телевизор по умолчанию должен быть выключен.

GREEN:

```cpp
bool IsTurnedOn() const noexcept {
    return is_turned_on_;
}

bool is_turned_on_ = false;
```

Тест проходит, потому что поведение стало правильным.

---

## ✅ Git после GREEN

Только когда тесты зелёные:

```bash
cd ~/cppbackend

git status
```

Добавить урок:

```bash
git add lessons/sprint_19_20/testing_and_debugging/lesson_5_9_tdd
```

Коммит:

```bash
git commit -m "add TDD TV default off example"
```

Пуш:

```bash
git push origin final-task-pr
```

---

## 🏁 Итог

Это первая минимальная TDD-итерация:

```text
требование
↓
тест
↓
падение
↓
минимальная реализация
↓
зелёный тест
```

Главный результат:

```text
TV выключен по умолчанию
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
