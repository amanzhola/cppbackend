# 🔗 UrlEncode

## 📌 Описание

Задача посвящена реализации и тестированию функции:

```cpp
std::string UrlEncode(std::string_view str);
```

Функция кодирует строку в URL-encoded формат:

```text
пробел → +
!      → %21
*      → %2A
/      → %2F
\n     → %0A
байт 255 → %FF
```

Это обратная задача к `UrlDecode`.

---

## 📂 Структура решения

```text
solution/
├── CMakeLists.txt
├── conanfile.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── urlencode.cpp
│   └── urlencode.h
└── tests/
    └── tests.cpp
```

---

## 🧠 Что изучается

| 📌 Тема            | 📖 Что проверяется                      | ✅ Итог                                            |
| ------------------ | --------------------------------------- | ------------------------------------------------- |
| URL Encode         | Преобразование строки в URL-safe формат | Корректное кодирование символов                   |
| Пробел             | Кодируется как `+`                      | Поддержка form-url-encoding                       |
| Reserved symbols   | Кодируются как `%XX`                    | Символы `/`, `?`, `&`, `=` и другие не ломают URL |
| Control characters | Кодируются как `%XX`                    | Управляющие символы не попадают в URL напрямую    |
| Байты `128+`       | Кодируются как `%XX`                    | Корректная работа с не-ASCII байтами              |
| Google Test        | Проверка обычных и граничных случаев    | Решение покрыто тестами                           |
| ABI `libstdc++11`  | Правильная сборка `gtest` через Conan   | Нет ошибок `[abi:cxx11]`                          |

---

## ⚙️ Логика функции

Функция проходит по строке слева направо.

Если символ — пробел:

```cpp
result += '+';
```

Если символ нужно закодировать:

```cpp
result += '%';
result += ToHexDigit(c / 16);
result += ToHexDigit(c % 16);
```

Иначе символ добавляется как есть:

```cpp
result += static_cast<char>(c);
```

---

## 🧩 Reserved symbols

В задаче reserved-символы кодируются через `%XX`.

Список:

```text
!#$&'()*+,/:;=?@[]
```

Например:

```text
! → %21
* → %2A
/ → %2F
? → %3F
= → %3D
& → %26
```

---

## ⚠️ Важный момент про `unsigned char`

Символы нужно обрабатывать как:

```cpp
unsigned char
```

Почему это важно:

```text
обычный char может быть signed
байты 128+ могут стать отрицательными
кодирование %80..%FF сломается
```

Правильный проход:

```cpp
for (unsigned char c : str)
```

---

## 🧪 Тесты

Тесты проверяют:

| Тест                                              | Что проверяет                                    |
| ------------------------------------------------- | ------------------------------------------------ |
| `EmptyString`                                     | Пустая строка                                    |
| `OrdinaryCharsAreNotEncoded`                      | Обычные символы не кодируются                    |
| `SpacesAreEncodedAsPlus`                          | Пробелы превращаются в `+`                       |
| `ReservedSymbolsArePercentEncoded`                | Reserved-символы кодируются в `%XX`              |
| `ControlCharactersArePercentEncoded`              | `\n`, `\x01`, `\x1F` кодируются                  |
| `CharactersWithCode128AndHigherArePercentEncoded` | Байты `128` и `255` кодируются как `%80` и `%FF` |

---

## 📦 conanfile.txt

```ini
[requires]
gtest/1.10.0

[generators]
cmake_multi
```

---

## 🧩 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.11)

project(urlencode CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(urlencode
    src/main.cpp
    src/urlencode.h
    src/urlencode.cpp
)

add_executable(tests
    tests/tests.cpp
    src/urlencode.h
    src/urlencode.cpp
)

target_link_libraries(tests PRIVATE CONAN_PKG::gtest)

add_test(NAME tests COMMAND tests)
```

---

## 🛠️ Сборка

Перейти в solution:

```bash
cd ~/cppbackend/sprint3/problems/urlencode/solution
```

Активировать Conan:

```bash
source ~/conan-venv/bin/activate
```

Собрать с правильным ABI:

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

Если бинарник находится в `bin`:

```bash
./bin/tests
```

Если в текущей папке:

```bash
./tests
```

Через CTest:

```bash
ctest --output-on-failure
```

Ожидаемый результат:

```text
100% tests passed
```

---

## 🏁 Итог

Главная цепочка задачи:

```text
UrlEncode
↓
обычные символы
↓
пробел → +
↓
reserved symbols → %XX
↓
control chars → %XX
↓
байты 128+ → %XX
↓
Google Test
↓
CTest
```

Главная идея:

```text
URL-encoding требует аккуратной обработки специальных символов, пробелов и байтов за пределами ASCII, поэтому такие случаи обязательно покрываются тестами.
```

---

## ⬅️ Назад

[Вернуться к Testing and Debugging](../../../lessons/sprint_19_20/testing_and_debugging/README.md)
