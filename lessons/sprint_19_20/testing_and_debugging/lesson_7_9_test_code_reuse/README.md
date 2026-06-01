# ♻️ Lesson 7/9 — Test Code Reuse

## 📌 Описание

Это продолжение серии TDD-уроков с классом `TV`.

В прошлом уроке появились:

* `TurnOn()`
* `GetChannel()`
* `std::optional<int>`
* поддержка вывода пользовательских типов для Boost.Test

Теперь добавляется новое требование:

```text
Телевизор можно выключить.
После выключения телевизор не показывает канал.
```

Также изучается важная техника тестирования:

```text
Fixtures (тестовые стенды)
```

которая позволяет убрать дублирование кода подготовки тестовых объектов.

---

## 🧠 Что изучается

| 📌 Тема                    | 📖 Что происходит                                     | ✅ Итог                                               |
| -------------------------- | ----------------------------------------------------- | ---------------------------------------------------- |
| `TurnOff()`                | Добавляется возможность выключать телевизор           | Поведение устройства становится полнее               |
| RED → GREEN                | Сначала тест падает, затем реализация исправляется    | Продолжается цикл TDD                                |
| Fixture                    | Общая подготовка объектов выносится в отдельный класс | Исчезает копипаст в тестах                           |
| `BOOST_FIXTURE_TEST_SUITE` | Набор тестов получает общий fixture                   | Тесты становятся короче                              |
| Наследование fixture       | Создаётся fixture для включённого телевизора          | Можно повторно использовать подготовленное состояние |
| Nested Test Suite          | Один набор тестов может содержать другой              | Удобная организация тестов                           |

---

## 📂 Структура проекта

```text
lesson_7_9_test_code_reuse/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── tv.h
│   └── tv.cpp
└── tests/
    └── tv_tests.cpp
```

---

## 📦 conanfile.txt

```ini
[requires]
boost/1.78.0

[generators]
cmake_multi
```

---

## ⚙️ CMakeLists.txt

```cmake
enable_testing()

cmake_minimum_required(VERSION 3.11)

project(test_code_reuse_example CXX)

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

Используется header-only вариант Boost.Test:

```cpp
#include <boost/test/included/unit_test.hpp>
```

Поэтому дополнительная линковка Boost.Test не требуется.

---

# 🔴 RED стадия

## src/tv.h

Начальная версия специально содержит ошибку.

Метод существует:

```cpp
void TurnOff() noexcept;
```

но ничего не делает.

```cpp
#pragma once

#include <optional>

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

    std::optional<int> GetChannel() const noexcept {
        if (is_turned_on_) {
            return channel_;
        }

        return std::nullopt;
    }

    void TurnOn() noexcept {
        is_turned_on_ = true;
    }

    void TurnOff() noexcept {
        // RED: специально ничего не делаем
    }

private:
    bool is_turned_on_ = false;
    int channel_ = 1;
};
```

Из-за этого тесты должны упасть.

---

## src/tv.cpp

```cpp
#include "tv.h"
```

---

## 🏗️ Сборка RED

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_7_9_test_code_reuse

rm -rf build

mkdir build

cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск RED

```bash
./tv_tests
```

Ожидаемо тесты падают.

Причина:

```cpp
tv.TurnOff();
```

ничего не меняет.

Телевизор остаётся включённым.

---

# 🟢 GREEN стадия

Исправить `src/tv.h`:

```cpp
void TurnOff() noexcept {
    is_turned_on_ = false;
}
```

Полная версия:

```cpp
#pragma once

#include <optional>

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

    std::optional<int> GetChannel() const noexcept {
        if (is_turned_on_) {
            return channel_;
        }

        return std::nullopt;
    }

    void TurnOn() noexcept {
        is_turned_on_ = true;
    }

    void TurnOff() noexcept {
        is_turned_on_ = false;
    }

private:
    bool is_turned_on_ = false;
    int channel_ = 1;
};
```

---

## Пересборка GREEN

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_7_9_test_code_reuse/build

cmake --build .
```

---

## Запуск GREEN

```bash
./tv_tests
```

Ожидаемо:

```text
Running 5 test cases...

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

# 🧪 Что такое Fixture

До появления fixture тесты часто выглядят так:

```cpp
TV tv;
tv.TurnOn();
```

И этот код повторяется снова и снова.

Например:

```cpp
BOOST_AUTO_TEST_CASE(shows_channel_1) {
    TV tv;
    tv.TurnOn();

    BOOST_TEST(tv.GetChannel() == 1);
}
```

И ещё раз:

```cpp
BOOST_AUTO_TEST_CASE(can_be_turned_off) {
    TV tv;
    tv.TurnOn();

    ...
}
```

Это дублирование.

---

## TVFixture

Для решения проблемы создаётся fixture:

```cpp
struct TVFixture {
    TV tv;
};
```

Теперь любой тест внутри:

```cpp
BOOST_FIXTURE_TEST_SUITE(TV_, TVFixture)
```

получает готовый объект:

```cpp
tv
```

без дополнительного создания.

---

## TurnedOnTVFixture

Следующий уровень переиспользования:

```cpp
struct TurnedOnTVFixture : TVFixture {
    TurnedOnTVFixture() {
        tv.TurnOn();
    }
};
```

Теперь каждый тест получает уже включённый телевизор.

---

## BOOST_FIXTURE_TEST_SUITE

Конструкция:

```cpp
BOOST_FIXTURE_TEST_SUITE(TV_, TVFixture)
```

означает:

```text
создать набор тестов TV_
и перед каждым тестом создавать новый TVFixture
```

Очень важно:

```text
каждый тест получает собственный fixture
```

Поэтому тесты не влияют друг на друга.

---

## Тест can_be_turned_off

Главный новый тест урока:

```cpp
BOOST_AUTO_TEST_CASE(can_be_turned_off) {
    tv.TurnOff();

    BOOST_TEST(!tv.IsTurnedOn());
    BOOST_TEST(tv.GetChannel() == std::nullopt);
}
```

Он проверяет:

```text
включённый телевизор можно выключить
↓
после выключения телевизор не показывает канал
```

---

## Почему fixture полезен

Без fixture:

```cpp
TV tv;
tv.TurnOn();
```

повторяется много раз.

С fixture:

```cpp
TurnedOnTVFixture
```

подготовка выполняется один раз и переиспользуется всеми тестами.

Получается:

```text
меньше копипаста
меньше ошибок
лучше читаемость
```

---

## RED → GREEN

RED:

```cpp
void TurnOff() noexcept {
}
```

Тест падает.

GREEN:

```cpp
void TurnOff() noexcept {
    is_turned_on_ = false;
}
```

Тест проходит.

Это правильная TDD-итерация.

---

## 🏁 Итог

В этом уроке добавлены:

```cpp
void TurnOff() noexcept;
```

и новая техника тестирования:

```text
Fixture
↓
Fixture inheritance
↓
BOOST_FIXTURE_TEST_SUITE
↓
повторное использование подготовки тестов
```

Главная мысль урока:

```text
Если несколько тестов одинаково подготавливают объект,
эту подготовку лучше вынести в fixture.
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
