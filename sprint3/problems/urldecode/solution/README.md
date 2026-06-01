# 🔗 UrlDecode

## 📌 Описание

Задача посвящена реализации и тестированию функции:

```cpp
std::string UrlDecode(std::string_view str);
```

Функция декодирует URL-encoded строку:

```text
%20 → пробел
%2F → /
+   → пробел
```

Если percent-последовательность некорректная, функция бросает исключение:

```cpp
std::invalid_argument
```

---

## 📂 Структура решения

```text
solution/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── urldecode.cpp
│   └── urldecode.h
└── tests/
    └── tests.cpp
```

---

## 🧠 Что изучается

| 📌 Тема    | 📖 Что проверяется                          | ✅ Итог                                              |
| ---------- | ------------------------------------------- | --------------------------------------------------- |
| URL Decode | Преобразование percent-encoded строк        | Корректное восстановление исходного текста          |
| `%XX`      | Декодирование шестнадцатеричных ASCII-кодов | `%20` превращается в пробел                         |
| `+`        | Специальное правило form-url-encoding       | `+` превращается в пробел                           |
| Исключения | Обработка некорректных последовательностей  | Ошибочные строки приводят к `std::invalid_argument` |
| Boost.Test | Проверка обычных и граничных случаев        | Решение покрыто тестами                             |
| CTest      | Запуск тестов через CMake                   | Тесты можно запускать автоматически                 |

---

## ⚙️ Логика функции

Функция идёт по строке слева направо.

Обычный символ добавляется как есть:

```cpp
result.push_back(ch);
```

Символ `+` превращается в пробел:

```cpp
result.push_back(' ');
```

Символ `%` требует две следующие шестнадцатеричные цифры:

```text
%20
```

Расчёт:

```text
2 * 16 + 0 = 32
```

Код `32` соответствует пробелу.

---

## 🧩 Основные helper-функции

### HexToInt

```cpp
int HexToInt(char ch);
```

Преобразует символ шестнадцатеричной цифры в число:

```text
'0'..'9' → 0..9
'A'..'F' → 10..15
'a'..'f' → 10..15
```

Если символ не является hex-цифрой, бросает:

```cpp
std::invalid_argument
```

---

### DecodePercentSequence

```cpp
char DecodePercentSequence(char first, char second);
```

Декодирует две hex-цифры после `%`.

Например:

```text
2 0 → 32 → ' '
```

---

## 🧪 Тесты

Тесты проверяют:

| Тест                                          | Что проверяет                              |
| --------------------------------------------- | ------------------------------------------ |
| `DecodeEmptyString`                           | Пустая строка                              |
| `DecodeStringWithoutPercentSequences`         | Строки без percent-кодов                   |
| `DecodeValidPercentSequencesInDifferentCases` | Валидные `%XX` в верхнем и нижнем регистре |
| `DecodePlusAsSpace`                           | Превращение `+` в пробел                   |
| `DecodeInvalidPercentSequences`               | Некорректные hex-символы                   |
| `DecodeIncompletePercentSequences`            | Неполные `%`-последовательности            |
| `DecodeUnescapedReservedCharactersAsIs`       | Неэкранированные reserved-символы          |

---

## ⚠️ Важное исправление теста

По условию:

```text
+ декодируется в пробел
```

Поэтому такой тест некорректен:

```cpp
BOOST_CHECK_EQUAL(
    UrlDecode("!#$&'()*+,/:;=?@[]"),
    "!#$&'()*+,/:;=?@[]"
);
```

Знак `+` в исходной строке будет превращён в пробел.

Правильный вариант — передать плюс как percent-encoded последовательность:

```cpp
BOOST_CHECK_EQUAL(
    UrlDecode("!#$&'()*%2B,/:;=?@[]"),
    "!#$&'()*+,/:;=?@[]"
);
```

---

## 🛠️ Сборка

Активировать Conan:

```bash
source ~/conan-venv/bin/activate
```

Собрать:

```bash
cd ~/cppbackend/sprint3/problems/urldecode/solution

rm -rf build
mkdir build
cd build

conan install .. --build=missing -s build_type=Release

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .
```

---

## ▶️ Запуск тестов

Запустить тестовый бинарник:

```bash
./tests
```

Запустить через CTest:

```bash
ctest --output-on-failure
```

Ожидаемый результат:

```text
100% tests passed, 0 tests failed out of 1
```

---

## ✅ Исправление CTest

Если `ctest` пишет:

```text
No tests were found
```

то в `CMakeLists.txt` нужно добавить:

```cmake
enable_testing()
add_test(NAME tests COMMAND tests)
```

После этого пересобрать:

```bash
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

---

## 🏁 Итог

Главная цепочка задачи:

```text
UrlDecode
↓
обработка обычных символов
↓
обработка +
↓
обработка %XX
↓
исключения на ошибках
↓
Boost.Test
↓
CTest
```

Главная идея:

```text
Тесты должны проверять не только обычные случаи, но и edge cases: пустые строки, плохие hex-символы, неполные percent-последовательности и специальные правила вроде + → пробел.
```
