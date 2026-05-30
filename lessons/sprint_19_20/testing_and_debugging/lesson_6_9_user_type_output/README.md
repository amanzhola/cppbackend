# 📺 Lesson 6/9 — User Type Output

## 📌 Описание

Это продолжение TDD-примера с классом `TV`.

В прошлом уроке проверялось, что телевизор выключен по умолчанию.

В этом уроке добавляются:

* состояние канала;
* `std::optional<int>`;
* `std::nullopt`;
* включение телевизора;
* вывод пользовательских типов в Boost.Test;
* настройка печати `std::optional<T>` при ошибках тестов.

---

## 🧠 Что изучается

| 📌 Тема              | 📖 Что происходит                                      | ✅ Итог                                            |
| -------------------- | ------------------------------------------------------ | ------------------------------------------------- |
| `std::optional<int>` | Канал может существовать или отсутствовать             | Не нужны фиктивные значения вроде `-1`            |
| `std::nullopt`       | Представляет отсутствие значения                       | Можно явно показать, что канал неизвестен         |
| RED → GREEN          | Сначала тесты падают, потом исправляется реализация    | Продолжается практика TDD                         |
| `operator<<`         | Добавляется поддержка вывода `std::optional<T>`        | Тесты становятся информативнее                    |
| Boost.Test           | Использует собственный механизм печати типов           | Можно получать понятные сообщения об ошибках      |
| `print_log_value`    | Настройка вывода пользовательских типов для Boost.Test | Boost.Test умеет показывать содержимое `optional` |

---

## 📂 Структура проекта

```text
lesson_6_9_user_type_output/
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

project(user_type_output_example CXX)

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

---

# 🔴 RED стадия

## src/tv.h

Начальная версия специально содержит ошибки логики:

```cpp
#pragma once

#include <optional>

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

    std::optional<int> GetChannel() const noexcept {
        return 42;
    }

    void TurnOn() noexcept {
    }

private:
    bool is_turned_on_ = false;
};
```

Проблемы:

```text
выключенный телевизор показывает канал 42
TurnOn ничего не делает
```

Поэтому тесты должны упасть.

---

## src/tv.cpp

```cpp
#include "tv.h"
```

---

## tests/tv_tests.cpp

Тесты проверяют:

```text
TV выключен по умолчанию
↓
выключенный TV не показывает канал
↓
optional корректно выводится
↓
после TurnOn телевизор показывает канал 1
```

---

## Важное исправление для optional_printing

В уроке есть распространённая ошибка.

Если написать:

```cpp
output << std::make_optional(42);
```

компилятор может не найти наш пользовательский `operator<<`.

Поэтому внутри теста обязательно нужно добавить:

```cpp
using test_util::operator<<;
```

Правильный вариант:

```cpp
BOOST_AUTO_TEST_CASE(optional_printing) {
    using namespace std::literals;
    using test_util::operator<<;

    std::ostringstream output;

    output << std::make_optional(42)
           << ' '
           << std::nullopt
           << ' '
           << std::optional<int>{};

    BOOST_TEST(std::move(output).str() == "42 nullopt nullopt"sv);
}
```

Без этой строки тест может не собраться.

---

## 🏗️ Сборка RED

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_6_9_user_type_output

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
GetChannel()
```

возвращает:

```cpp
42
```

даже если телевизор выключен.

А:

```cpp
TurnOn()
```

ничего не делает.

---

# 🟢 GREEN стадия

Заменить `src/tv.h` на:

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

private:
    bool is_turned_on_ = false;
    int channel_ = 1;
};
```

---

## Пересборка GREEN

```bash
cd ~/cppbackend/lessons/sprint_19_20/testing_and_debugging/lesson_6_9_user_type_output/build

cmake --build .
```

---

## Запуск GREEN

```bash
./tv_tests
```

Ожидаемо:

```text
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

## Что такое std::optional

Тип:

```cpp
std::optional<int>
```

означает:

```text
либо есть int
либо значения нет
```

Например:

```cpp
return 1;
```

или:

```cpp
return std::nullopt;
```

---

## Что такое std::nullopt

```cpp
std::nullopt
```

означает:

```text
значения нет
```

Для выключенного телевизора:

```cpp
GetChannel()
```

должен вернуть именно:

```cpp
std::nullopt
```

---

## Почему нужен optional

Плохой вариант:

```cpp
int GetChannel();
```

Тогда пришлось бы возвращать:

```cpp
0
```

или:

```cpp
-1
```

Хотя это не отсутствие значения.

Правильнее:

```cpp
std::optional<int>
```

---

## Почему Boost.Test ругается на optional

Проблемный код:

```cpp
BOOST_TEST(tv.GetChannel() == 1);
```

При ошибке Boost.Test пытается вывести реальные значения:

```text
actual != expected
```

Но стандартный:

```cpp
std::optional<int>
```

не имеет встроенного вывода в поток.

Поэтому появляются ошибки вида:

```text
Type has to implement operator<< to be printable
```

---

## Решение

Добавляем:

```cpp
namespace test_util
```

и собственный:

```cpp
operator<<(std::ostream&, const std::optional<T>&)
```

Также добавляем специализации:

```cpp
boost::test_tools::tt_detail::print_log_value
```

После этого Boost.Test умеет печатать:

```text
42
nullopt
```

и вывод ошибок становится понятным.

---

## 🏁 Итог

Урок показывает три важных идеи:

```text
std::optional
↓
вывод пользовательских типов
↓
продолжение цикла TDD
```

По итогам урока:

```text
выключенный TV не показывает канал
включённый TV показывает канал 1
Boost.Test умеет печатать optional
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
