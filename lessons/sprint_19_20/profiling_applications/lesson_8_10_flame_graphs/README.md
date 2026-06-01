# 🔥 Lesson 8/10 — Flame Graphs

## 📌 Описание

В этом упражнении изучается построение FlameGraph для C++ приложения.

FlameGraph помогает визуально увидеть, где программа тратит больше всего времени. Вместо чтения длинного текстового отчёта можно открыть интерактивный SVG-файл и увидеть широкие блоки функций, в которых было больше всего сэмплов.

Типовая цепочка выглядит так:

```text
perf record
↓
perf.data
↓
perf script
↓
stackcollapse-perf.pl
↓
flamegraph.pl
↓
graph.svg
```

---

## ⚠️ Важно

Это упражнение рекомендуется выполнять на полноценной Linux-машине или на виртуальной машине в Yandex Cloud.

В WSL часто возникают проблемы:

```text
perf not found for kernel ... microsoft-standard-WSL2
```

или ограничения доступа к perf events.

Поэтому рабочий сценарий для этого урока:

```text
YC / Linux VM
```

---

## 🧠 Что изучается

| 📌 Тема                 | 📖 Что происходит                                 | ✅ Итог                                     |
| ----------------------- | ------------------------------------------------- | ------------------------------------------ |
| `perf record`           | Собирает сэмплы выполнения программы              | Создаётся файл `perf.data`                 |
| `perf report`           | Показывает текстовый отчёт по профилю             | Можно проверить горячие функции            |
| `perf script`           | Преобразует `perf.data` в текстовые стеки вызовов | Данные становятся пригодны для FlameGraph  |
| `stackcollapse-perf.pl` | Сворачивает стеки в формат FlameGraph             | Получается компактное представление стеков |
| `flamegraph.pl`         | Создаёт интерактивный SVG                         | Появляется файл `graph.svg`                |
| FlameGraph              | Показывает функции в виде цветных прямоугольников | Ширина блока показывает долю сэмплов       |

---

## 📂 Структура урока

```text
lesson_8_10_flame_graphs/
├── main.cpp
├── README.md
├── app
├── perf.data
├── graph.svg
└── FlameGraph/
    ├── stackcollapse-perf.pl
    └── flamegraph.pl
```

В репозиторий обычно стоит добавлять:

```text
main.cpp
README.md
```

Не стоит коммитить:

```text
app
perf.data
graph.svg
FlameGraph/
```

Это артефакты сборки, профилирования и скачанный внешний инструмент.

---

## 🖥️ Подключение к YC

Из Windows `cmd`:

```cmd
ssh -i C:\keys_cplus_plus\id_rsa almusha@94.131.92.240
```

---

## 📁 Перейти в папку урока

```bash
cd ~/cppbackend/lessons/sprint_19_20/profiling_applications/lesson_8_10_flame_graphs
```

Если папки ещё нет:

```bash
cd ~/cppbackend/lessons/sprint_19_20/profiling_applications
mkdir -p lesson_8_10_flame_graphs
cd lesson_8_10_flame_graphs
```

---

## 🧩 Создать `main.cpp`

```bash
nano main.cpp
```

Вставить код:

```cpp
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

namespace {

volatile double global_result = 0.0;

void FastFunction() {
    double result = 0.0;

    for (int i = 1; i <= 500000; ++i) {
        result += i * 0.000001;
    }

    global_result += result;
}

void MediumFunction() {
    double result = 0.0;

    for (int i = 1; i <= 3000000; ++i) {
        result += std::sqrt(static_cast<double>(i));
    }

    global_result += result;
}

void SlowFunction() {
    double result = 0.0;

    for (int i = 1; i <= 12000000; ++i) {
        result += std::sin(static_cast<double>(i)) * std::cos(static_cast<double>(i));
    }

    global_result += result;
}

void MemoryFunction() {
    std::vector<int> numbers(3000000);

    std::iota(numbers.begin(), numbers.end(), 1);

    std::shuffle(
        numbers.begin(),
        numbers.end(),
        std::mt19937{std::random_device{}()}
    );

    long long sum = 0;

    for (int value : numbers) {
        sum += value;
    }

    global_result += static_cast<double>(sum);
}

void ApplicationScenario() {
    FastFunction();
    MediumFunction();
    SlowFunction();
    MemoryFunction();
}

}  // namespace

int main() {
    std::cout << "Program started" << std::endl;

    const auto start = std::chrono::steady_clock::now();

    for (int iteration = 0; iteration < 100; ++iteration) {
        ApplicationScenario();
    }

    const auto finish = std::chrono::steady_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        finish - start
    );

    std::cout << "Program finished" << std::endl;
    std::cout << "Duration: " << duration.count() << " ms" << std::endl;
    std::cout << "Global result: " << global_result << std::endl;

    return 0;
}
```

Сохранить:

```text
Ctrl + O
Enter
Ctrl + X
```

---

## ⚙️ Собрать программу

```bash
g++ -O2 -g -fno-omit-frame-pointer main.cpp -o app
```

Проверить:

```bash
ls -la app
```

---

## ▶️ Проверить обычный запуск

```bash
./app
```

---

## 📊 Записать профиль через perf

На YC лучше сразу запускать через `sudo`:

```bash
sudo perf record -F 99 -g ./app
```

Пояснение:

```text
-F 99   частота сэмплирования 99 раз в секунду
-g      записывать стек вызовов
./app   профилируемая программа
```

Проверить файл:

```bash
ls -la perf.data
```

Размер должен быть больше нуля.

---

## 📄 Проверить отчёт perf

Интерактивно:

```bash
sudo perf report -f
```

Выйти:

```text
q
```

Текстовый вариант:

```bash
sudo perf report -f --stdio | head -80
```

---

## 🧰 Установить инструменты FlameGraph

```bash
sudo apt update
sudo apt install -y git perl
```

Клонировать FlameGraph:

```bash
git clone https://github.com/brendangregg/FlameGraph
```

Проверить:

```bash
ls FlameGraph
```

Внутри должны быть:

```text
stackcollapse-perf.pl
flamegraph.pl
```

---

## 🔥 Построить `graph.svg`

Основная команда:

```bash
sudo perf script -i perf.data | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > graph.svg
```

Проверить:

```bash
ls -la graph.svg
```

Посмотреть первые строки:

```bash
head -20 graph.svg
```

Начало должно быть примерно таким:

```xml
<?xml version="1.0" standalone="no"?>
<svg ...
```

---

## 🪟 Скачать SVG на Windows

Выйти из SSH:

```bash
exit
```

В Windows `cmd` выполнить:

```cmd
scp -i C:\keys_cplus_plus\id_rsa almusha@94.131.92.240:/home/almusha/cppbackend/lessons/sprint_19_20/profiling_applications/lesson_8_10_flame_graphs/graph.svg C:\Users\админ\Desktop\graph.svg
```

---

## 🌐 Открыть FlameGraph

В Windows `cmd`:

```cmd
start C:\Users\админ\Desktop\graph.svg
```

Или открыть через проводник:

```text
Рабочий стол
↓
graph.svg
```

Двойной клик откроет SVG в браузере.

---

## 🔎 Как читать FlameGraph

Во FlameGraph:

```text
низ           начало стека вызовов
верх          более глубокие вызовы
ширина блока  сколько сэмплов попало в функцию и её потомков
цвет          обычно декоративный
```

Главное правило:

```text
широкий блок = много времени
узкий блок = мало времени
```

Если широкий блок называется:

```text
SlowFunction
```

значит эта функция является горячей точкой.

---

## 🧪 Полный блок команд для копипаста

```bash
cd ~/cppbackend/lessons/sprint_19_20/profiling_applications
mkdir -p lesson_8_10_flame_graphs
cd lesson_8_10_flame_graphs

nano main.cpp

g++ -O2 -g -fno-omit-frame-pointer main.cpp -o app

./app

sudo perf record -F 99 -g ./app

ls -la perf.data

sudo perf report -f --stdio | head -80

sudo apt update
sudo apt install -y git perl

git clone https://github.com/brendangregg/FlameGraph

sudo perf script -i perf.data | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > graph.svg

ls -la graph.svg

head -20 graph.svg
```

---

## ♻️ Если `FlameGraph` уже существует

Если `git clone` выдаёт:

```text
fatal: destination path 'FlameGraph' already exists
```

повторно клонировать не нужно.

Просто выполнить:

```bash
sudo perf script -i perf.data | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > graph.svg
```

---

## 🧼 Перед коммитом

Удалить артефакты:

```bash
rm -f app perf.data graph.svg
rm -rf FlameGraph
```

Проверить:

```bash
git status
```

В коммит должны попасть только:

```text
main.cpp
README.md
```

и обновлённый:

```text
../README.md
```

---

## 🏁 Итог

Главная цепочка урока:

```text
C++ программа
↓
perf record
↓
perf.data
↓
perf script
↓
stackcollapse-perf.pl
↓
flamegraph.pl
↓
graph.svg
↓
открытие в браузере
```

Главная идея:

```text
FlameGraph превращает статистический профиль в наглядную карту горячих участков программы.
```

---

## ⬅️ Назад

[Вернуться к Profiling Applications](../README.md)

[Вернуться в общий README репозитория](../../../../README.md)
