# 🧪 Lesson 3/8 — More About Catch2

## 📌 Описание

Этот урок продолжает тему Catch2.

Если в прошлом уроке основное внимание было на настройке `CMake`, `Conan`, `CTest` и `catch_discover_tests`, то здесь изучаются возможности самих тестов Catch2:

* проверка исключений;
* матчеры;
* `Predicate`;
* собственные матчеры;
* сравнение чисел с плавающей точкой;
* ручные моки.

Эти инструменты особенно полезны для будущих задач про коллизии, координаты, игровую статистику и проверку сложной логики.

---

## 🧠 Что изучается

| 📌 Тема             | 📖 Что происходит                                                           | ✅ Итог                                            |
| ------------------- | --------------------------------------------------------------------------- | ------------------------------------------------- |
| Исключения          | Проверяются ошибки через `CHECK_THROWS`, `CHECK_THROWS_AS`, `CHECK_NOTHROW` | Можно тестировать ошибочные сценарии              |
| Матчеры             | Используются `CHECK_THAT`, `Contains`, `Predicate`                          | Проверки становятся читаемее                      |
| Собственные матчеры | Создаётся `IsPermutation`                                                   | Можно описывать свои правила проверки             |
| Floating point      | Используются `WithinAbs`, `WithinRel`, `WithinULP`                          | Координаты и дробные числа сравниваются корректно |
| Моки                | Создаётся `CountingComparator`                                              | Можно проверять взаимодействие объектов           |
| CTest               | Catch2-тесты подключаются через `catch_discover_tests`                      | Тесты можно запускать через `ctest`               |

---

## 📂 Структура проекта

```text
lesson_3_8_more_about_catch2/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   └── algorithms.h
└── tests/
    └── catch2_features_tests.cpp
```

---

## 🛠️ Создание проекта

```bash
cd ~/cppbackend/lessons/sprint_19_20/collision_detection_and_statistics

mkdir -p lesson_3_8_more_about_catch2

cd lesson_3_8_more_about_catch2

mkdir -p src tests

touch CMakeLists.txt conanfile.txt
touch src/algorithms.h
touch tests/catch2_features_tests.cpp
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

## 🧩 src/algorithms.h

```cpp
#pragma once

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

template <class T>
T GetMax(const std::vector<T>& elems) {
    using namespace std::literals;

    if (elems.empty()) {
        throw std::invalid_argument("Attempt to get max of empty array"s);
    }

    return *std::max_element(elems.begin(), elems.end());
}

inline std::vector<int> GetRandomVector(std::size_t size) {
    std::vector<int> values(size);

    std::iota(values.begin(), values.end(), 0);

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::shuffle(values.begin(), values.end(), generator);

    return values;
}

template <class T>
class CountingComparator {
public:
    bool operator()(const T& left, const T& right) const {
        ++counter_;
        return left < right;
    }

    std::size_t GetCounter() const {
        return counter_;
    }

private:
    mutable std::size_t counter_ = 0;
};
```

---

## 🧪 tests/catch2_features_tests.cpp

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "../src/algorithms.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <numbers>
#include <string>
#include <vector>

using namespace std::literals;

using Catch::Matchers::Contains;
using Catch::Matchers::Predicate;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
using Catch::Matchers::WithinULP;

template <typename Range>
struct IsPermutationMatcher : Catch::Matchers::MatcherGenericBase {
    IsPermutationMatcher(Range range)
        : range_{std::move(range)} {
        std::sort(std::begin(range_), std::end(range_));
    }

    IsPermutationMatcher(IsPermutationMatcher&&) = default;

    template <typename OtherRange>
    bool match(OtherRange other) const {
        using std::begin;
        using std::end;

        std::sort(begin(other), end(other));

        return std::equal(begin(range_), end(range_), begin(other), end(other));
    }

    std::string describe() const override {
        return "Is permutation of: "s + Catch::rangeToString(range_);
    }

private:
    Range range_;
};

template <typename Range>
IsPermutationMatcher<Range> IsPermutation(Range&& range) {
    return IsPermutationMatcher<Range>{std::forward<Range>(range)};
}

TEST_CASE("Exceptions can be checked", "[exceptions]") {
    CHECK_THROWS(GetMax<int>({}));
    CHECK_THROWS_AS(GetMax<int>({}), std::invalid_argument);
    CHECK_THROWS_WITH(GetMax<int>({}), "Attempt to get max of empty array"s);

    CHECK_NOTHROW(GetMax<int>({1, 2, 3}));
    CHECK(GetMax<int>({1, 2, 3}) == 3);
}

TEST_CASE("Container matchers make checks readable", "[matchers]") {
    std::vector<int> values = {1, 2, 3, 4, 5};

    CHECK_THAT(values, Contains(3));
    CHECK_THAT(values, !Contains(100));
    CHECK_THAT(values, Contains(3) && !Contains(100));

    auto greater_than_ten = [](int value) {
        return value > 10;
    };

    CHECK_THAT(values, !Contains(Predicate<int>(greater_than_ten)));
}

TEST_CASE("Custom matcher checks permutation", "[custom-matcher]") {
    std::vector<int> values = {3, 1, 2};

    CHECK_THAT(values, IsPermutation(std::vector{1, 2, 3}));
}

TEST_CASE("Floating point values should be compared approximately", "[floating-point]") {
    double value = 0.1;

    CHECK_FALSE(value + value + value + value + value + value + value + value + value + value == 1.0);

    CHECK_THAT(value * 10, WithinAbs(1.0, 1e-15));
    CHECK_THAT(value * 10, WithinRel(1.0, 1e-12));

    CHECK_FALSE(std::asin(0.5) == std::numbers::pi / 6);
    CHECK_THAT(std::asin(0.5), WithinULP(std::numbers::pi / 6, 1));
}

TEST_CASE("Mock comparator counts sort comparisons", "[mock]") {
    const std::size_t size = 1000;

    auto values = GetRandomVector(size);

    CountingComparator<int> comparator;

    std::sort(values.begin(), values.end(), std::ref(comparator));

    CHECK(std::is_sorted(values.begin(), values.end()));
    CHECK(comparator.GetCounter() < 2 * size * std::log2(size));
}
```

---

## ⚙️ CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.11)

project(more_about_catch2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(catch2_features_tests
    tests/catch2_features_tests.cpp
    src/algorithms.h
)

target_compile_definitions(catch2_features_tests PRIVATE
    CATCH_CONFIG_MAIN
)

target_include_directories(catch2_features_tests PRIVATE
    ${CONAN_INCLUDE_DIRS}
)

target_link_libraries(catch2_features_tests PRIVATE
    ${CONAN_LIBS}
)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

catch_discover_tests(catch2_features_tests)
```

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

## ▶️ Запуск тестов

```bash
./bin/catch2_features_tests
```

---

## 📋 Список тестов

```bash
./bin/catch2_features_tests --list-tests
```

---

## 🏷️ Запуск по тегу

```bash
./bin/catch2_features_tests "[exceptions]"
./bin/catch2_features_tests "[matchers]"
./bin/catch2_features_tests "[floating-point]"
./bin/catch2_features_tests "[mock]"
```

---

## 🔍 Запуск через CTest

```bash
ctest --output-on-failure
```

---

## 🧨 Исключения

Вместо ручного `try/catch`:

```cpp
bool has_exception = false;

try {
    GetMax<int>({});
} catch (const std::invalid_argument&) {
    has_exception = true;
}

CHECK(has_exception);
```

в Catch2 лучше писать:

```cpp
CHECK_THROWS(GetMax<int>({}));
CHECK_THROWS_AS(GetMax<int>({}), std::invalid_argument);
CHECK_NOTHROW(GetMax<int>({1, 2, 3}));
```

---

## 🧲 Матчеры

Обычная проверка контейнера:

```cpp
CHECK(std::find(v.begin(), v.end(), 5) != v.end());
```

читается хуже, чем:

```cpp
CHECK_THAT(v, Contains(5));
```

Матчеры можно комбинировать:

```cpp
CHECK_THAT(v, Contains(5) && !Contains(11));
```

---

## 🧠 Predicate

`Predicate` позволяет передать собственное условие:

```cpp
auto greater_than_ten = [](int value) {
    return value > 10;
};

CHECK_THAT(values, !Contains(Predicate<int>(greater_than_ten)));
```

---

## 🧩 Собственный матчер

Собственный матчер `IsPermutation` позволяет проверять, что два контейнера являются перестановками друг друга:

```cpp
CHECK_THAT(values, IsPermutation(std::vector{1, 2, 3}));
```

Это полезно, когда порядок элементов не важен, но состав должен совпадать.

---

## 🌊 Floating point

Числа с плавающей точкой нельзя надёжно сравнивать через `==`:

```cpp
CHECK(result == expected);
```

Лучше использовать:

```cpp
CHECK_THAT(result, WithinAbs(expected, epsilon));
CHECK_THAT(result, WithinRel(expected, epsilon));
CHECK_THAT(result, WithinULP(expected, max_ulps));
```

Для будущих задач про коллизии это особенно важно, потому что координаты часто имеют тип `double`.

---

## 🎭 Mock

`CountingComparator` — простой ручной мок.

Он считает, сколько раз алгоритм вызвал компаратор:

```cpp
CountingComparator<int> comparator;

std::sort(values.begin(), values.end(), std::ref(comparator));

CHECK(comparator.GetCounter() < 2 * size * std::log2(size));
```

`std::ref` нужен, чтобы `std::sort` работал с исходным объектом компаратора, а не с его копией.

---

## 🏁 Итог

Этот урок даёт набор инструментов для серьёзных тестов:

```text
исключения
матчеры
собственные матчеры
floating point
моки
CTest
```

Для будущих задач особенно важны:

```text
WithinAbs / WithinRel — для координат
CHECK_THROWS_AS — для ошибок правил игры
собственные матчеры — для проверки геометрии
моки — для проверки взаимодействий объектов
```

---

## ⬅️ Назад

[Вернуться к Collision Detection and Statistics](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
