# 🧩 HTML Decode

## 📌 Описание

Задача `htmldecode` реализует декодирование HTML-сущностей в обычные символы.

Программа читает строку из стандартного ввода, применяет функцию `HtmlDecode` и выводит результат.

---

## 🧠 Что декодируется

| HTML-сущность       | Символ |
| ------------------- | ------ |
| `&lt;` / `&LT;`     | `<`    |
| `&gt;` / `&GT;`     | `>`    |
| `&amp;` / `&AMP;`   | `&`    |
| `&apos;` / `&APOS;` | `'`    |
| `&quot;` / `&QUOT;` | `"`    |

Точка с запятой `;` после сущности может быть, а может отсутствовать.

Например:

```text
Johnson&amp;Johnson
Johnson&ampJohnson
```

оба варианта декодируются в:

```text
Johnson&Johnson
```

---

## 📂 Структура решения

```text
solution/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── main.cpp
│   ├── htmldecode.h
│   └── htmldecode.cpp
└── tests/
    └── tests.cpp
```

---

## 📦 conanfile.txt

```ini
[requires]
catch2/3.1.0

[generators]
cmake_multi
```

---

## ⚙️ CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.11)

project(htmldecode CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(htmldecode
    src/main.cpp
    src/htmldecode.h
    src/htmldecode.cpp
)

add_executable(tests
    tests/tests.cpp
    src/htmldecode.h
    src/htmldecode.cpp
)

target_link_libraries(tests PRIVATE CONAN_PKG::catch2)
```

---

## 🧩 src/htmldecode.h

```cpp
#pragma once

#include <string>
#include <string_view>

std::string HtmlDecode(std::string_view str);
```

---

## 🧩 src/htmldecode.cpp

```cpp
#include "htmldecode.h"

#include <array>
#include <string>
#include <string_view>

namespace {

struct Entity {
    std::string_view name;
    char decoded_char;
};

constexpr std::array<Entity, 10> ENTITIES = {{
    {"lt", '<'},
    {"LT", '<'},
    {"gt", '>'},
    {"GT", '>'},
    {"amp", '&'},
    {"AMP", '&'},
    {"apos", '\''},
    {"APOS", '\''},
    {"quot", '"'},
    {"QUOT", '"'},
}};

bool StartsWith(std::string_view text, std::string_view prefix) {
    return text.size() >= prefix.size()
        && text.substr(0, prefix.size()) == prefix;
}

}  // namespace

std::string HtmlDecode(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    for (size_t position = 0; position < str.size();) {
        if (str[position] != '&') {
            result.push_back(str[position]);
            ++position;
            continue;
        }

        bool entity_found = false;
        const std::string_view after_ampersand = str.substr(position + 1);

        for (const Entity& entity : ENTITIES) {
            if (StartsWith(after_ampersand, entity.name)) {
                result.push_back(entity.decoded_char);

                position += 1 + entity.name.size();

                if (position < str.size() && str[position] == ';') {
                    ++position;
                }

                entity_found = true;
                break;
            }
        }

        if (!entity_found) {
            result.push_back(str[position]);
            ++position;
        }
    }

    return result;
}
```

---

## 🧩 src/main.cpp

```cpp
#include <iostream>
#include <string>

#include "htmldecode.h"

int main() {
    std::string input;
    std::getline(std::cin, input);

    std::cout << HtmlDecode(input) << std::endl;
}
```

---

## 🧪 tests/tests.cpp

```cpp
#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

using namespace std::literals;

TEST_CASE("String without html entities") {
    CHECK(HtmlDecode("Hello world"sv) == "Hello world"s);
    CHECK(HtmlDecode("Johnson&Johnson"sv) == "Johnson&Johnson"s);
}

TEST_CASE("Empty string") {
    CHECK(HtmlDecode(""sv) == ""s);
}

TEST_CASE("Lowercase html entities with semicolon") {
    CHECK(HtmlDecode("&lt;"sv) == "<"s);
    CHECK(HtmlDecode("&gt;"sv) == ">"s);
    CHECK(HtmlDecode("&amp;"sv) == "&"s);
    CHECK(HtmlDecode("&apos;"sv) == "'"s);
    CHECK(HtmlDecode("&quot;"sv) == "\""s);
}

TEST_CASE("Lowercase html entities without semicolon") {
    CHECK(HtmlDecode("&lt"sv) == "<"s);
    CHECK(HtmlDecode("&gt"sv) == ">"s);
    CHECK(HtmlDecode("&amp"sv) == "&"s);
    CHECK(HtmlDecode("&apos"sv) == "'"s);
    CHECK(HtmlDecode("&quot"sv) == "\""s);
}

TEST_CASE("Uppercase html entities") {
    CHECK(HtmlDecode("&LT;"sv) == "<"s);
    CHECK(HtmlDecode("&GT;"sv) == ">"s);
    CHECK(HtmlDecode("&AMP;"sv) == "&"s);
    CHECK(HtmlDecode("&APOS;"sv) == "'"s);
    CHECK(HtmlDecode("&QUOT;"sv) == "\""s);
}

TEST_CASE("Mixed case entities are not decoded") {
    CHECK(HtmlDecode("&aPos;"sv) == "&aPos;"s);
    CHECK(HtmlDecode("&Amp;"sv) == "&Amp;"s);
    CHECK(HtmlDecode("&QuOt;"sv) == "&QuOt;"s);
}

TEST_CASE("Entities at beginning middle and end") {
    CHECK(HtmlDecode("&lt;tag&gt;"sv) == "<tag>"s);
    CHECK(HtmlDecode("M&amp;M&APOSs"sv) == "M&M's"s);
    CHECK(HtmlDecode("&quot;hello&quot;"sv) == "\"hello\""s);
}

TEST_CASE("Unknown entities are kept unchanged") {
    CHECK(HtmlDecode("&abracadabra"sv) == "&abracadabra"s);
    CHECK(HtmlDecode("hello&unknown;world"sv) == "hello&unknown;world"s);
}

TEST_CASE("Decoded symbols are not decoded again") {
    CHECK(HtmlDecode("&amp;lt;"sv) == "&lt;"s);
    CHECK(HtmlDecode("&AMPamp;"sv) == "&amp;"s);
}

TEST_CASE("Incomplete entities") {
    CHECK(HtmlDecode("&"sv) == "&"s);
    CHECK(HtmlDecode("&l"sv) == "&l"s);
    CHECK(HtmlDecode("&ap"sv) == "&ap"s);
    CHECK(HtmlDecode("&quo"sv) == "&quo"s);
}

TEST_CASE("Examples from statement") {
    CHECK(HtmlDecode("Johnson&amp;Johnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&ampJohnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&AMP;Johnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&AMPJohnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&Johnson"sv) == "Johnson&Johnson"s);
}
```

---

## 🏗️ Сборка

```bash
cd ~/cppbackend/sprint3/problems/htmldecode/solution

rm -rf build
mkdir build
cd build

source ~/conan-venv/bin/activate

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## 🧪 Запуск тестов

В этой задаче CMake может положить исполняемые файлы прямо в папку `build/`, а не в `build/bin/`.

Поэтому сначала проверь:

```bash
ls -la
```

Если видишь файл:

```text
tests
```

запускай так:

```bash
./tests
```

Ожидаемый результат:

```text
All tests passed (37 assertions in 11 test cases)
```

Если в другой среде бинарники окажутся в `bin/`, тогда команда будет:

```bash
./bin/tests
```

---

## ▶️ Ручная проверка

Если в `build/` есть файл:

```text
htmldecode
```

запускай так:

```bash
echo 'M&amp;M&APOSs' | ./htmldecode
```

Ожидаемый вывод:

```text
M&M's
```

Если бинарник лежит в `bin/`, тогда:

```bash
echo 'M&amp;M&APOSs' | ./bin/htmldecode
```

---

## 🧠 Главная идея решения

Функция `HtmlDecode` проходит по строке один раз.

Если текущий символ не `&`, он просто копируется в результат.

Если встретился `&`, функция проверяет известные HTML-сущности:

```text
lt, LT
gt, GT
amp, AMP
apos, APOS
quot, QUOT
```

Если сущность найдена, в результат добавляется настоящий символ.

Если после сущности стоит `;`, она пропускается.

Если сущность неизвестная, символ `&` остаётся как обычный символ.

---

## 🏁 Итог

В этом упражнении реализованы:

| ✅ Часть       | 📖 Что сделано                              |
| ------------- | ------------------------------------------- |
| Алгоритм      | Однопроходное декодирование HTML-сущностей  |
| CLI-программа | Чтение строки из `stdin` и вывод результата |
| Тесты         | 37 проверок в 11 test case                  |
| Сборка        | Conan + CMake + Catch2                      |

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../../../../lessons/sprint_19_20/testing_and_debugging/README.md)

[Вернуться в общий README репозитория](../../../../README.md)
