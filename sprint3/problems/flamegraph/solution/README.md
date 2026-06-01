# 🔥 FlameGraph

## 📌 Описание

Задача посвящена автоматическому построению FlameGraph для серверного приложения.

В отличие от учебного мини-урока, где профилировалась простая C++ программа, здесь профилируется уже запущенный серверный процесс.

Скрипт `shoot.py` выполняет полный цикл:

```text
запуск сервера
↓
запуск perf record по PID сервера
↓
генерация HTTP-нагрузки через curl
↓
остановка perf
↓
создание perf.data
↓
преобразование perf.data в folded.stacks
↓
создание graph.svg
```

---

## 📂 Структура solution

```text
solution/
├── FlameGraph/
│   ├── flamegraph.pl
│   └── stackcollapse-perf.pl
├── README.md
└── shoot.py
```

---

## 🧠 Что делает `shoot.py`

| 📌 Часть скрипта     | 📖 Назначение                                                       |
| -------------------- | ------------------------------------------------------------------- |
| `start_server()`     | Читает команду запуска сервера из аргументов командной строки       |
| `run()`              | Запускает внешний процесс через `subprocess.Popen`                  |
| `start_perf()`       | Запускает `perf record -o perf.data -g -p PID` для процесса сервера |
| `make_shots()`       | Отправляет HTTP-запросы в сервер через `curl`                       |
| `stop_perf()`        | Корректно завершает `perf record` через `SIGINT`                    |
| `stop()`             | Останавливает серверный процесс                                     |
| `build_flamegraph()` | Преобразует `perf.data` в `graph.svg` через скрипты FlameGraph      |

---

## 🔫 Нагрузка

В скрипте используется список URL:

```python
AMMUNITION = [
    'localhost:8080/api/v1/maps/map1',
    'localhost:8080/api/v1/maps'
]
```

Каждый запрос отправляется через `curl`.

Количество запросов:

```python
SHOOT_COUNT = 100
```

Пауза между запросами:

```python
COOLDOWN = 0.1
```

---

## 🔥 Профилирование

Скрипт запускает `perf` так:

```text
perf record -o perf.data -g -p <PID сервера>
```

Где:

| Параметр       | Значение                                     |
| -------------- | -------------------------------------------- |
| `-o perf.data` | Явно указать файл для записи профиля         |
| `-g`           | Записывать стек вызовов                      |
| `-p PID`       | Профилировать уже запущенный процесс сервера |

---

## 🖼️ Построение FlameGraph

Функция `build_flamegraph()` выполняет цепочку:

```text
perf script -i perf.data
↓
FlameGraph/stackcollapse-perf.pl
↓
folded.stacks
↓
FlameGraph/flamegraph.pl
↓
graph.svg
```

В результате появляется файл:

```text
graph.svg
```

Его можно открыть в браузере.

---

## ⚙️ Как запускать

Формат запуска:

```bash
python3 shoot.py '<команда запуска сервера>'
```

Например:

```bash
python3 shoot.py './game_server ../data/config.json'
```

Если сервер требует дополнительные параметры, они передаются внутри строки:

```bash
python3 shoot.py './game_server --config-file config.json --www-root static'
```

---

## ⚠️ Важно про YC / Linux

Эта задача должна выполняться на полноценной Linux-машине или на YC VM.

В WSL `perf` может не работать корректно из-за ограничений ядра и доступа к perf events.

Если `perf record` требует права администратора, запускать нужно через `sudo` или настроить права для perf.

---

## ✅ Ожидаемый результат

После успешного запуска появляются файлы:

```text
perf.data
folded.stacks
graph.svg
```

Главный результат задачи:

```text
graph.svg
```

Это интерактивный FlameGraph, который показывает, где сервер тратит больше всего времени во время обработки HTTP-запросов.

---

## 🧹 Перед коммитом

Обычно не нужно коммитить временные файлы:

```text
perf.data
folded.stacks
graph.svg
```

В solution должны оставаться:

```text
FlameGraph/
README.md
shoot.py
```

---

## 🏁 Итог

Задача показывает полный цикл серверного профилирования:

```text
сервер
↓
нагрузка
↓
perf
↓
perf.data
↓
FlameGraph
↓
graph.svg
```

Главная идея:

```text
FlameGraph удобен не только для учебных функций, но и для анализа реального backend-сервера под нагрузкой.
```
