# ✅ Lesson 2/9 — Boost.Test

## 📌 Описание

В этом уроке изучается первый полноценный фреймворк модульного тестирования для C++ — Boost.Test.

Проект собирается через:

```text
Conan
↓
CMake
↓
Boost.Test
```

В качестве примера тестируется функция определения високосного года:

```cpp
bool IsLeapYear(int year);
```

---

## 🧠 Что изучается

| 📌 Тема              | 📖 Что изучается                  | ✅ Итог                           |
| -------------------- | --------------------------------- | -------------------------------- |
| Boost.Test           | Фреймворк модульного тестирования | Написание автоматических тестов  |
| Conan                | Менеджер зависимостей C++         | Подключение Boost                |
| CMake                | Система сборки проекта            | Сборка приложения и тестов       |
| CTest                | Запуск набора тестов              | Автоматическая проверка          |
| BOOST_CHECK          | Проверка условий                  | Проверка корректности результата |
| BOOST_AUTO_TEST_CASE | Создание тестовых сценариев       | Организация тестов               |

---

## 📂 Структура проекта

```text
lesson_2_9_boost_test/
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
    └── boost_test_main.cpp
```

---

## 📦 Зависимости через Conan

Файл:

```text
conanfile.txt
```

Содержимое:

```ini
[requires]
boost/1.78.0

[generators]
cmake_multi
```

Boost устанавливается автоматически через Conan.

---

## ⚙️ Сборка

Активировать Conan:

```bash
source ~/conan-venv/bin/activate
```

Проверить:

```bash
conan --version
```

Создать папку сборки:

```bash
mkdir -p build
cd build
```

Установить зависимости:

```bash
conan install .. --build=missing -s build_type=Release
```

Сконфигурировать проект:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Собрать:

```bash
cmake --build .
```

---

## ▶️ Запуск приложения

```bash
./bin/application
```

Пример:

```text
Enter year: 2024
2024 is a leap year
```

---

## 🧪 Запуск тестов

Запуск напрямую:

```bash
./boost_test_example
```

Ожидаемый результат:

```text
Running 1 test case...

*** No errors detected
```

Через CTest:

```bash
ctest --output-on-failure
```

Успешный результат:

```text
100% tests passed
```

---

## 🔍 Основные элементы Boost.Test

Имя тестового модуля:

```cpp
#define BOOST_TEST_MODULE leap year application tests
```

Создание теста:

```cpp
BOOST_AUTO_TEST_CASE(IsLeapYear_test)
```

Проверка условия:

```cpp
BOOST_CHECK(IsLeapYear(2024));
```

Проверка на ложь:

```cpp
BOOST_CHECK(!IsLeapYear(2023));
```

Сравнение значений:

```cpp
BOOST_CHECK_EQUAL(actual, expected);
```

Остановка теста при ошибке:

```cpp
BOOST_REQUIRE(condition);
```

---

## ⚠️ Исправление проблемы линковки

Первоначально использовалось:

```cpp
#include <boost/test/unit_test.hpp>
```

Это требует отдельной линковки библиотеки:

```text
boost_unit_test_framework
```

Из-за этого возникала ошибка:

```text
undefined reference to boost::unit_test...
```

---

## ✅ Рабочее решение

Был использован header-only вариант:

```cpp
#define BOOST_TEST_MODULE leap year application tests
#include <boost/test/included/unit_test.hpp>
```

В этом случае реализация Boost.Test включается прямо в тестовый файл.

Поэтому строка:

```cmake
target_link_libraries(
    boost_test_example
    PRIVATE
    CONAN_PKG::boost
)
```

становится не нужна.

---

## 🧩 Итоговый рабочий тест

```cpp
#define BOOST_TEST_MODULE leap year application tests
#include <boost/test/included/unit_test.hpp>

#include "../src/leap_year.h"

BOOST_AUTO_TEST_CASE(IsLeapYear_test) {
    BOOST_CHECK(IsLeapYear(2020));
    BOOST_CHECK(!IsLeapYear(2021));
    BOOST_CHECK(!IsLeapYear(2022));
    BOOST_CHECK(!IsLeapYear(2023));
    BOOST_CHECK(IsLeapYear(2024));
    BOOST_CHECK(!IsLeapYear(1900));
    BOOST_CHECK(IsLeapYear(2000));
}
```

---

## 🏁 Итог

Главная цепочка урока:

```text
Conan
↓
Boost
↓
CMake
↓
Boost.Test
↓
CTest
↓
Автоматическая проверка кода
```

Главная идея:

```text
Юнит-тесты позволяют автоматически проверять корректность бизнес-логики и находить ошибки до запуска программы пользователем.
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
