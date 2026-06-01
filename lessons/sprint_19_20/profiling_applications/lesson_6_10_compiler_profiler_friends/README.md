# 🤝 Lesson 6/10 — Compiler + Profiler = Friends

## 📌 Описание

Мини-упражнение показывает, как профилировать C++ программу с помощью компилятора `g++`, флага `-pg` и профилировщика `gprof`.

В прошлом упражнении время выполнения измерялось вручную через `LOG_DURATION`. Здесь подход другой: компилятор сам добавляет в программу служебный код профилирования, программа создаёт файл `gmon.out`, а `gprof` строит отчёт по времени выполнения функций.

---

## 🧠 Что изучается

| 📌 Тема                | 📖 Что происходит                                             | ✅ Итог                                                |
| ---------------------- | ------------------------------------------------------------- | ----------------------------------------------------- |
| `-pg`                  | Флаг компилятора добавляет профилировочное инструментирование | Программа начинает собирать данные для `gprof`        |
| `gmon.out`             | Файл появляется после запуска программы, собранной с `-pg`    | В нём хранятся данные профилирования                  |
| `gprof`                | Анализирует исполняемый файл и `gmon.out`                     | Показывает, где программа тратит больше всего времени |
| Flat profile           | Таблица функций по времени выполнения                         | Можно найти горячие точки                             |
| Call graph             | Граф вызовов функций                                          | Видно, кто кого вызывает                              |
| `gprof2dot + graphviz` | Преобразует отчёт `gprof` в картинку                          | Можно получить визуальный граф вызовов                |

---

## 📂 Структура упражнения

```text
lesson_6_10_compiler_profiler_friends/
├── CMakeLists.txt
├── main.cpp
└── README.md
```

После сборки дополнительно появятся:

```text
build/
gmon.out
profile_report.txt
output.png
```

---

## 🧩 main.cpp

В файле `main.cpp` находятся функции с разной нагрузкой:

| 📌 Функция            | 📖 Что делает                                                               |
| --------------------- | --------------------------------------------------------------------------- |
| `FastFunction`        | Выполняет простой цикл сложения                                             |
| `MediumFunction`      | Считает квадратные корни через `std::sqrt`                                  |
| `SlowFunction`        | Использует `std::sin` и `std::cos`                                          |
| `VerySlowFunction`    | Создаёт `std::vector`, заполняет его через `std::iota` и суммирует значения |
| `ApplicationScenario` | Последовательно вызывает все функции                                        |

---

## ⚙️ CMakeLists.txt

В `CMakeLists.txt` важно добавить `-pg` и на этапе компиляции, и на этапе линковки:

```cmake
target_compile_options(
    lesson_6_10_compiler_profiler_friends
    PRIVATE
    -pg
    -O0
)

target_link_options(
    lesson_6_10_compiler_profiler_friends
    PRIVATE
    -pg
)
```

`-O0` отключает оптимизации, чтобы компилятор не выбрасывал или сильно не менял код во время учебного профилирования.

---

## 🛠️ Сборка через CMake

Перейти в папку урока:

```bash
cd ~/cppbackend/lessons/sprint_19_20/profiling_applications/lesson_6_10_compiler_profiler_friends
```

Создать папку сборки:

```bash
mkdir -p build
cd build
```

Запустить CMake:

```bash
cmake ..
```

Собрать проект:

```bash
cmake --build .
```

---

## ▶️ Запуск программы

Находясь в папке `build`, запусти:

```bash
./lesson_6_10_compiler_profiler_friends
```

После запуска должен появиться файл:

```bash
ls
```

И среди файлов должен быть:

```text
gmon.out
```

---

## 📊 Получить отчёт gprof

Находясь в папке `build`:

```bash
gprof ./lesson_6_10_compiler_profiler_friends gmon.out > profile_report.txt
```

Посмотреть начало отчёта:

```bash
head -80 profile_report.txt
```

Открыть полный отчёт:

```bash
less profile_report.txt
```

Выйти из `less`:

```text
q
```

---

## 🔎 Что искать в отчёте

Главная часть отчёта называется:

```text
Flat profile
```

В ней важны колонки:

| 📌 Колонка           | 📖 Значение                                             |
| -------------------- | ------------------------------------------------------- |
| `% time`             | Процент времени, который программа провела в функции    |
| `cumulative seconds` | Накопленное время                                       |
| `self seconds`       | Собственное время функции                               |
| `calls`              | Количество вызовов                                      |
| `self s/call`        | Среднее собственное время одного вызова                 |
| `total s/call`       | Среднее время одного вызова вместе с дочерними вызовами |
| `name`               | Имя функции                                             |

Если функция занимает большой процент времени, она является вероятной горячей точкой.

---

## 🔗 Посмотреть call graph

Граф вызовов показывает, кто кого вызывает:

```text
main
└── ApplicationScenario
    ├── FastFunction
    ├── MediumFunction
    ├── SlowFunction
    └── VerySlowFunction
```

Команда для просмотра части `Call graph`:

```bash
grep -A80 "Call graph" profile_report.txt
```

Или через `less`:

```bash
less profile_report.txt
```

Потом внутри `less`:

```text
/Call graph
Enter
```

---

## 🖼️ Визуализация через gprof2dot

В Ubuntu 24.04 обычный `pip3 install gprof2dot` может завершиться ошибкой:

```text
error: externally-managed-environment
```

Это нормально: система запрещает ставить Python-пакеты глобально через `pip`.

Лучший вариант — использовать `pipx`.

Установить зависимости:

```bash
sudo apt update
sudo apt install -y pipx graphviz
pipx ensurepath
```

Обновить окружение текущего терминала:

```bash
source ~/.profile
```

Установить `gprof2dot`:

```bash
pipx install gprof2dot
```

Проверить:

```bash
gprof2dot --help
```

Построить картинку:

```bash
gprof ./lesson_6_10_compiler_profiler_friends gmon.out | gprof2dot | dot -Tpng -o output.png
```

Проверить:

```bash
ls
```

Открыть текущую папку в Windows:

```bash
explorer.exe .
```

---

## 🧪 Быстрая сборка без CMake

Можно собрать напрямую через `g++`:

```bash
cd ~/cppbackend/lessons/sprint_19_20/profiling_applications/lesson_6_10_compiler_profiler_friends
g++ -O3 -pg main.cpp -o app
```

Запустить:

```bash
./app
```

Получить отчёт:

```bash
gprof app gmon.out > profile_report.txt
```

Посмотреть начало:

```bash
head -80 profile_report.txt
```

---

## 🏁 Итог

Главная цепочка урока:

```text
g++ + -pg
↓
запуск программы
↓
gmon.out
↓
gprof
↓
profile_report.txt
↓
поиск горячих точек
```

Главная идея:

```text
Компилятор помогает профайлеру собрать данные,
а профайлер помогает разработчику найти слабые места программы.
```

---

## ⬅️ Назад

[Вернуться к Profiling Applications](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
