# Sprint 18_20 — Theme 2_4 — Lesson 4_5 — Four Golden Metrics

---

## Тема

| 📌 Что изучается в этом уроке по четырём золотым метрикам мониторинга | 📖 Подробное описание темы урока                                                                             | 🧠 Почему это важно для backend-сервера и production-мониторинга           | ✅ Итог                                                             |
| --------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------ | -------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| Четыре золотые метрики мониторинга                                    | В этом уроке изучаются Latency, Traffic, Errors и Saturation, а также экспорт метрик приложения в Prometheus | Эти метрики помогают понять скорость, нагрузку, ошибки и насыщение системы | После урока понятно, как собирать и визуализировать golden metrics |

---

## Полный путь проекта

| 📌 Окружение | 📖 Полный путь проекта                                                              | 🧠 Когда использовать                                       | ✅ Итог                |
| ------------ | ----------------------------------------------------------------------------------- | ----------------------------------------------------------- | --------------------- |
| WSL          | `/home/ubuntu/cppbackend/lessons/sprint_18_20_theme_2_4_lesson_4_5_golden_metrics`  | Использовать в WSL или Linux-среде с пользователем `ubuntu` | Путь для WSL известен |
| Yandex Cloud | `/home/almusha/cppbackend/lessons/sprint_18_20_theme_2_4_lesson_4_5_golden_metrics` | Использовать в Yandex Cloud с пользователем `almusha`       | Путь для YC известен  |

```text
/home/ubuntu/cppbackend/lessons/sprint_18_20_theme_2_4_lesson_4_5_golden_metrics
```

```text
Для Yandex Cloud:

/home/almusha/cppbackend/lessons/sprint_18_20_theme_2_4_lesson_4_5_golden_metrics
```

---

## Цель практики

| 📌 Главная цель практики                         | 📖 Подробное описание действия                                                                                    | 🧠 Практический смысл                                                          | ✅ Итог                                 |
| ------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ | -------------------------------------- |
| Экспортировать метрики приложения в Prometheus   | Нужно создать exporter, который читает логи приложения, превращает их в Prometheus-метрики и отдаёт на `/metrics` | Prometheus не читает произвольные stdout-логи напрямую, поэтому нужен exporter | Метрики приложения доступны Prometheus |
| Визуализировать четыре золотые метрики в Grafana | Нужно построить панели Latency, Traffic, Errors и Saturation                                                      | Grafana показывает состояние приложения и сервера в удобном dashboard          | Golden metrics отображаются в Grafana  |

```text
Научиться экспортировать метрики приложения в Prometheus и визуализировать четыре золотые метрики мониторинга в Grafana.
```

---

## Четыре золотые метрики

| 📌 Метрика | 📖 Что означает               | 🧠 Что показывает на практике                                    | ✅ Итог                      |
| ---------- | ----------------------------- | ---------------------------------------------------------------- | --------------------------- |
| Latency    | Задержка ответа сервера       | Показывает, сколько времени в среднем занимает обработка запроса | Видна скорость ответа       |
| Traffic    | Количество запросов в секунду | Показывает интенсивность входящей нагрузки                       | Видна нагрузка запросами    |
| Errors     | Процент ошибочных запросов    | Показывает долю HTTP-ответов с кодами 4xx и 5xx                  | Видны ошибки                |
| Saturation | Загрузка системы              | Показывает CPU, Disk и RAM Usage                                 | Видна насыщенность ресурсов |

```text
Latency    → задержка ответа сервера
Traffic    → количество запросов в секунду
Errors     → процент ошибочных запросов
Saturation → загрузка системы
```

---

## Структура проекта

| 📌 Элемент проекта                           | 📖 Что находится внутри                    | 🧠 Для чего используется                                          | ✅ Итог                        |
| -------------------------------------------- | ------------------------------------------ | ----------------------------------------------------------------- | ----------------------------- |
| `README.md`                                  | Документация урока                         | Описание exporter, Prometheus, Grafana, golden metrics и проверки | README готов                  |
| `commands/01_install_python_client.sh`       | Скрипт установки Python-клиента Prometheus | Устанавливает библиотеку `prometheus_client`                      | Python-клиент установлен      |
| `commands/02_run_exporter_with_test_logs.sh` | Скрипт запуска exporter с тестовыми логами | Проверяет exporter без реального сервера                          | Exporter можно протестировать |
| `commands/03_copy_prometheus_config.sh`      | Скрипт обновления Prometheus-конфига       | Копирует `prometheus.yml` в контейнер и перезапускает Prometheus  | Конфиг применяется            |
| `commands/04_check_exporter.sh`              | Скрипт проверки exporter                   | Проверяет доступность `/metrics` на порту 9200                    | Exporter проверяется          |
| `commands/05_run_server_with_exporter.sh`    | Скрипт запуска сервера вместе с exporter   | Передаёт stdout сервера в stdin exporter через pipe               | Сервер связан с exporter      |
| `configs/prometheus.yml`                     | Конфигурация Prometheus                    | Добавляет targets `node` и `server`                               | Prometheus настроен           |
| `exporter/test_logs.txt`                     | Тестовые JSON-логи                         | Используются для проверки exporter                                | Тестовые данные есть          |
| `exporter/web_exporter.py`                   | Python exporter                            | Читает JSON-логи, создаёт метрики и отдаёт `/metrics`             | Exporter реализован           |
| `grafana/dashboard_queries.md`               | Запросы для dashboard Grafana              | Хранит PromQL-запросы для golden metrics                          | Запросы для Grafana сохранены |
| `static/load_simulator.html`                 | HTML-страница симуляции нагрузки           | Помогает генерировать запросы к приложению                        | Нагрузка может симулироваться |

```text
sprint_18_20_theme_2_4_lesson_4_5_golden_metrics/
├── README.md
├── commands/
│   ├── 01_install_python_client.sh
│   ├── 02_run_exporter_with_test_logs.sh
│   ├── 03_copy_prometheus_config.sh
│   ├── 04_check_exporter.sh
│   └── 05_run_server_with_exporter.sh
├── configs/
│   └── prometheus.yml
├── exporter/
│   ├── test_logs.txt
│   └── web_exporter.py
├── grafana/
│   └── dashboard_queries.md
└── static/
    └── load_simulator.html
```

---

## [1] Экспортируем метрики

| 📌 Почему нужен exporter                               | 📖 Подробное описание                                                                | 🧠 Практический смысл                                                                         | ✅ Итог                                            |
| ------------------------------------------------------ | ------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------- | ------------------------------------------------- |
| Prometheus не читает произвольные stdout-логи напрямую | Prometheus ожидает HTTP endpoint `/metrics`, где метрики отдаются в понятном формате | Нужно создать отдельный exporter, который превратит JSON-логи приложения в Prometheus-метрики | Exporter нужен между приложением и Prometheus     |
| Python exporter                                        | Создаётся файл `exporter/web_exporter.py`                                            | Он читает stdin, разбирает JSON, создаёт метрики и отдаёт их на `/metrics`                    | Exporter реализует мост между логами и Prometheus |
| Порт exporter                                          | Exporter работает на порту `9200`                                                    | Prometheus сможет обращаться к `localhost:9200`                                               | Метрики приложения доступны на 9200               |

```text
Prometheus не читает произвольные логи из stdout напрямую.
```

```text
exporter/web_exporter.py
```

```text
Он:

читает JSON-логи из stdin
разбирает строки
создаёт Prometheus-метрики
отдаёт их на /metrics
работает на порту 9200
```

---

## Установка Python-библиотеки

| 📌 Скрипт установки Python-библиотеки    | 📖 Команда внутри скрипта                         | 🧠 Что устанавливается                                           | ✅ Итог                         |
| ---------------------------------------- | ------------------------------------------------- | ---------------------------------------------------------------- | ------------------------------ |
| `./commands/01_install_python_client.sh` | `python3 -m pip install --user prometheus_client` | Устанавливается Python-библиотека для создания Prometheus-метрик | `prometheus_client` установлен |

```text
./commands/01_install_python_client.sh
```

```bash
python3 -m pip install --user prometheus_client
```

---

## Метрики exporter

| 📌 Метрика exporter       | 📖 Тип метрики | 🧠 Что считает                  | ✅ Итог                     |
| ------------------------- | -------------- | ------------------------------- | -------------------------- |
| `webexporter_good_lines`  | Counter        | Успешно разобранные JSON-строки | Хорошие строки считаются   |
| `webexporter_wrong_lines` | Counter        | Строки с ошибками разбора       | Ошибочные строки считаются |

```python
good_lines = prom.Counter('webexporter_good_lines', 'Good JSON records')
wrong_lines = prom.Counter('webexporter_wrong_lines', 'Wrong JSON records')
```

```text
webexporter_good_lines  → успешно разобранные JSON-строки
webexporter_wrong_lines → строки с ошибками разбора
```

---

## Основная метрика приложения

| 📌 Основная метрика приложения      | 📖 Тип метрики   | 🧠 Что измеряет                                                      | ✅ Итог                                     |
| ----------------------------------- | ---------------- | -------------------------------------------------------------------- | ------------------------------------------ |
| `webserver_request_duration`        | Histogram        | Измеряет длительность HTTP-запросов с labels `code` и `content_type` | Можно считать latency, traffic и errors    |
| `webserver_request_duration_bucket` | Histogram bucket | Хранит распределение запросов по bucket-диапазонам                   | Можно анализировать распределение задержек |
| `webserver_request_duration_sum`    | Histogram sum    | Хранит суммарное время всех запросов                                 | Используется для расчёта средней latency   |
| `webserver_request_duration_count`  | Histogram count  | Хранит количество запросов                                           | Используется для traffic и errors          |

```python
response_time = prom.Histogram(
    'webserver_request_duration',
    'Response time',
    ['code', 'content_type'],
    buckets=(.001, .002, .005, .010, .020, .050, .100, .200, .500, float("inf"))
)
```

```text
Это histogram-метрика.

Она создаёт:

webserver_request_duration_bucket
webserver_request_duration_sum
webserver_request_duration_count
```

---

## Почему labels code и content_type

| 📌 Label             | 📖 Что означает                         | 🧠 Почему полезен                                            | ✅ Итог                            |
| -------------------- | --------------------------------------- | ------------------------------------------------------------ | --------------------------------- |
| `code`               | HTTP-код ответа                         | Позволяет отличать успешные запросы от ошибок                | Ошибки можно считать отдельно     |
| `content_type`       | Тип ответа                              | Позволяет различать HTML, API, картинки и другие типы ответа | Ответы можно группировать по типу |
| IP-адрес клиента     | Не рекомендуется использовать как label | Создаёт слишком много временных рядов                        | Лучше не добавлять                |
| Точное время запроса | Не рекомендуется использовать как label | Каждое значение времени создаёт новый ряд                    | Лучше не добавлять                |

```text
code         → позволяет отличать успешные запросы от ошибок
content_type → позволяет различать HTML, API, картинки и другие типы ответа
```

```text
Не стоит использовать:

IP-адрес клиента
точное время запроса

Потому что это создаёт слишком много временных рядов.
```

---

## Проверка exporter

| 📌 Шаг проверки exporter  | 💻 Команда                                                              | 📖 Что делает команда                                        | ✅ Итог                      |
| ------------------------- | ----------------------------------------------------------------------- | ------------------------------------------------------------ | --------------------------- |
| Запуск с тестовыми логами | `./commands/02_run_exporter_with_test_logs.sh`                          | Запускает exporter и подаёт в него тестовые JSON-логи        | Exporter стартует           |
| Проверка метрик           | `curl http://localhost:9200/metrics \| grep webserver_request_duration` | Проверяет, появились ли метрики `webserver_request_duration` | Метрики приложения доступны |

```text
./commands/02_run_exporter_with_test_logs.sh
```

```bash
curl http://localhost:9200/metrics | grep webserver_request_duration
```

---

## [2] Связываем сервер и Prometheus

| 📌 Что настраивается | 📖 Подробное описание                                            | 🧠 Для чего нужно                                                | ✅ Итог                        |
| -------------------- | ---------------------------------------------------------------- | ---------------------------------------------------------------- | ----------------------------- |
| Prometheus config    | В `prometheus.yml` добавляются два scrape job: `node` и `server` | Prometheus будет собирать системные метрики и метрики приложения | Prometheus связан с exporter  |
| `node` target        | `localhost:9100`                                                 | Метрики Node Exporter                                            | Системные метрики собираются  |
| `server` target      | `localhost:9200`                                                 | Метрики Python exporter                                          | Метрики приложения собираются |

```yaml
global:
  scrape_interval: 5s
  evaluation_interval: 5s

scrape_configs:
  - job_name: 'node'
    static_configs:
      - targets: [ 'localhost:9100' ]

  - job_name: 'server'
    static_configs:
      - targets: [ 'localhost:9200' ]
```

---

## Обновление Prometheus

| 📌 Скрипт обновления Prometheus           | 📖 Команды внутри                                                                                        | 🧠 Что делают команды                                            | ✅ Итог                       |
| ----------------------------------------- | -------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------- | ---------------------------- |
| `./commands/03_copy_prometheus_config.sh` | `sudo docker cp configs/prometheus.yml prom:/etc/prometheus/prometheus.yml` и `sudo docker restart prom` | Копируют новый конфиг в контейнер Prometheus и перезапускают его | Prometheus применяет targets |

```text
./commands/03_copy_prometheus_config.sh
```

```bash
sudo docker cp configs/prometheus.yml prom:/etc/prometheus/prometheus.yml
sudo docker restart prom
```

---

## Проверка targets

| 📌 Что открыть             | 📖 Что должно быть        | 🧠 Что это означает                                   | ✅ Итог             |
| -------------------------- | ------------------------- | ----------------------------------------------------- | ------------------ |
| `http://<ip>:9090/targets` | Targets `node` и `server` | Prometheus видит Node Exporter и application exporter | Targets подключены |
| `node`                     | `localhost:9100`          | Системные метрики доступны                            | Node target UP     |
| `server`                   | `localhost:9200`          | Метрики приложения доступны                           | Server target UP   |

```text
http://<ip>:9090/targets
```

```text
Должны быть targets:

node   → localhost:9100
server → localhost:9200
```

---

## Запуск сервера вместе с exporter

| 📌 Принцип запуска                | 📖 Команда                                                                           | 🧠 Что делает pipe `\|`                       | ✅ Итог                                |
| --------------------------------- | ------------------------------------------------------------------------------------ | --------------------------------------------- | ------------------------------------- |
| Сервер передаёт stdout в exporter | `sudo docker run --rm -p 80:8080 my_http_server \| python3 exporter/web_exporter.py` | Pipe передаёт stdout сервера в stdin exporter | Логи сервера превращаются в метрики   |
| `std::cout`                       | Сервер должен писать JSON-логи в stdout                                              | Exporter читает именно stdin                  | Логи доступны exporter                |
| `auto_flush = true`               | Должен быть включён auto flush                                                       | Логи сразу отправляются в pipe                | Exporter получает данные без задержки |

```bash
sudo docker run --rm -p 80:8080 my_http_server | python3 exporter/web_exporter.py
```

```text
Pipe | передаёт stdout сервера в stdin exporter.
```

```text
Важно, чтобы сервер писал JSON-логи в:

std::cout

и чтобы был включён:

auto_flush = true
```

---

## Traffic / Rate

| 📌 Golden metric | 📖 PromQL-запрос                                   | 🧠 Смысл запроса                                             | ✅ Итог            |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ | ----------------- |
| Traffic / Rate   | `sum(rate(webserver_request_duration_count[15s]))` | Среднее количество запросов в секунду за последние 15 секунд | Traffic считается |

```text
sum(rate(webserver_request_duration_count[15s]))
```

```text
Смысл:

среднее количество запросов в секунду за последние 15 секунд
```

---

## Latency

| 📌 Golden metric | 📖 PromQL-запрос                                                                                           | 🧠 Смысл запроса                                    | ✅ Итог            |
| ---------------- | ---------------------------------------------------------------------------------------------------------- | --------------------------------------------------- | ----------------- |
| Latency          | `sum(rate(webserver_request_duration_sum[15s])) / sum(rate(webserver_request_duration_count[15s])) * 1000` | Средняя длительность одного запроса в миллисекундах | Latency считается |

```text
sum(rate(webserver_request_duration_sum[15s])) / sum(rate(webserver_request_duration_count[15s])) * 1000
```

```text
Смысл:

средняя длительность одного запроса в миллисекундах
```

---

## Errors

| 📌 Golden metric | 📖 PromQL-запрос                                                                                                            | 🧠 Смысл запроса                                           | ✅ Итог                  |
| ---------------- | --------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------- | ----------------------- |
| Errors           | `sum(rate(webserver_request_duration_count{code=~"[45].."}[15s])) / sum(rate(webserver_request_duration_count[15s])) * 100` | Процент запросов с HTTP-кодами 4xx или 5xx                 | Errors считаются        |
| `[45]..`         | Регулярное выражение                                                                                                        | Код начинается с 4 или 5, после этого идут любые две цифры | 4xx и 5xx отфильтрованы |

```text
sum(rate(webserver_request_duration_count{code=~"[45].."}[15s])) / sum(rate(webserver_request_duration_count[15s])) * 100
```

```text
Смысл:

процент запросов с HTTP-кодами 4xx или 5xx
```

```text
[45]..

означает:

код начинается с 4 или 5
после этого идут любые две цифры
```

---

## Saturation

| 📌 Saturation-панель | 📖 PromQL-запрос                                                                                              | 🧠 Что показывает                            | ✅ Итог                |
| -------------------- | ------------------------------------------------------------------------------------------------------------- | -------------------------------------------- | --------------------- |
| CPU Usage            | `avg(1 - rate(node_cpu_seconds_total{mode="idle"}[1m])) * 100`                                                | Загрузку CPU в процентах                     | CPU saturation видна  |
| Disk Usage           | `(1 - node_filesystem_free_bytes{device="/dev/vda1"} / node_filesystem_size_bytes{device="/dev/vda1"}) * 100` | Занятое место на диске в процентах           | Disk saturation видна |
| RAM Usage            | `(1 - node_memory_MemFree_bytes / node_memory_MemTotal_bytes) * 100`                                          | Использование оперативной памяти в процентах | RAM saturation видна  |

```text
Saturation уже была реализована в прошлой практике через панели:

CPU Usage
Disk Usage
RAM Usage
```

```text
CPU Usage:

avg(1 - rate(node_cpu_seconds_total{mode="idle"}[1m])) * 100
```

```text
Disk Usage:

(1 - node_filesystem_free_bytes{device="/dev/vda1"} / node_filesystem_size_bytes{device="/dev/vda1"}) * 100
```

```text
RAM Usage:

(1 - node_memory_MemFree_bytes / node_memory_MemTotal_bytes) * 100
```

---

## Все панели Grafana

| 📌 Панель dashboard | 📖 Что показывает             | 🧠 К какой метрике относится | ✅ Итог              |
| ------------------- | ----------------------------- | ---------------------------- | ------------------- |
| CPU Usage           | Загрузку процессора           | Saturation                   | Панель CPU есть     |
| Disk Usage          | Использование диска           | Saturation                   | Панель Disk есть    |
| RAM Usage           | Использование памяти          | Saturation                   | Панель RAM есть     |
| Rate                | Количество запросов в секунду | Traffic                      | Панель Rate есть    |
| Latency             | Среднюю задержку ответа       | Latency                      | Панель Latency есть |
| Errors              | Процент ошибочных запросов    | Errors                       | Панель Errors есть  |

```text
На dashboard должно быть шесть панелей:

CPU Usage
Disk Usage
RAM Usage
Rate
Latency
Errors
```

---

## Полезные URL

| 📌 Сервис мониторинга | 🌐 URL                     | 📖 Что открывается                    | ✅ Итог                 |
| --------------------- | -------------------------- | ------------------------------------- | ---------------------- |
| Node Exporter         | `http://<ip>:9100/metrics` | Системные метрики Linux-сервера       | Node metrics доступны  |
| Prometheus            | `http://<ip>:9090/`        | Веб-интерфейс Prometheus              | Prometheus UI доступен |
| Grafana               | `http://<ip>:3000/`        | Веб-интерфейс Grafana                 | Grafana UI доступен    |
| Application exporter  | `http://<ip>:9200/metrics` | Метрики приложения из Python exporter | App metrics доступны   |

```text
Node Exporter:
http://<ip>:9100/metrics

Prometheus:
http://<ip>:9090/

Grafana:
http://<ip>:3000/

Application exporter:
http://<ip>:9200/metrics
```

---

## Итог

| 📌 Что сделано в этой практике по Four Golden Metrics | 📖 Подробное описание результата                                                                     | 🧠 Почему это важно                                        | ✅ Итог                 |
| ----------------------------------------------------- | ---------------------------------------------------------------------------------------------------- | ---------------------------------------------------------- | ---------------------- |
| Создан Python exporter                                | Написан exporter, который читает JSON-логи из stdin, разбирает строки и отдаёт метрики на `/metrics` | Prometheus получил источник метрик приложения              | Exporter создан        |
| Созданы Counter-метрики                               | Добавлены `webexporter_good_lines` и `webexporter_wrong_lines`                                       | Можно видеть количество успешных и ошибочных строк разбора | Counter-метрики есть   |
| Создана Histogram-метрика                             | Добавлена `webserver_request_duration` с labels `code` и `content_type`                              | На её основе считаются Traffic, Latency и Errors           | Histogram-метрика есть |
| Prometheus подключён к exporter                       | В `prometheus.yml` добавлен job `server` с target `localhost:9200`                                   | Prometheus начал собирать метрики приложения               | Exporter подключён     |
| Grafana получила панели Rate, Latency и Errors        | Для golden metrics добавлены PromQL-запросы                                                          | Dashboard показывает состояние приложения                  | Панели созданы         |
| Четыре золотые метрики разобраны на практике          | Изучены Latency, Traffic, Errors и Saturation                                                        | Можно анализировать работу сервера и приложения            | Golden metrics понятны |

```text
В этой практике:

создан Python exporter
созданы Counter-метрики
создана Histogram-метрика
Prometheus подключён к exporter
Grafana получила панели Rate, Latency и Errors
четыре золотые метрики разобраны на практике
```
