# 💣 Ammo

## 📌 Описание

Задача посвящена подготовке файлов для нагрузочного тестирования через Yandex Tank.

В рамках задания необходимо создать:

```text
ammo.txt
load.yaml
load_stress.yaml
```

Эти файлы используются для генерации HTTP-нагрузки на игровой сервер.

---

## 📂 Структура решения

```text
solution/
├── ammo.txt
├── load.yaml
├── load_stress.yaml
└── README.md
```

---

## 🧠 Что изучается

| 📌 Тема          | 📖 Что изучается                         | ✅ Итог                                                 |
| ---------------- | ---------------------------------------- | ------------------------------------------------------ |
| ammo.txt         | Подготовка набора HTTP-запросов          | Формирование тестовых сценариев                        |
| load.yaml        | Конфигурация обычного нагрузочного теста | Проверка производительности под умеренной нагрузкой    |
| load_stress.yaml | Конфигурация стресс-теста                | Проверка поведения сервера под экстремальной нагрузкой |
| RPS              | Requests Per Second                      | Управление интенсивностью запросов                     |
| URI Ammo         | Формат запросов для Phantom              | Подготовка патронов для Yandex Tank                    |

---

## 🔫 Файл ammo.txt

Содержимое:

```text
[Connection: close]
[Host: cppserver]
[Cookie: None]
/api/v1/maps
/api/v1/maps/map1
/api/v1/maps/map2
/api/v1/game/join
/api/v1/game/players
/api/v1/game/state
```

---

### Разбор содержимого

Закрывать соединение после ответа:

```text
Connection: close
```

Тестовый сервер:

```text
Host: cppserver
```

Куки не используются:

```text
Cookie: None
```

Тестируемые endpoint'ы:

```text
/api/v1/maps
/api/v1/maps/map1
/api/v1/maps/map2
/api/v1/game/join
/api/v1/game/players
/api/v1/game/state
```

Каждая строка соответствует отдельному HTTP GET запросу.

---

## ⚙️ Файл load.yaml

Содержимое:

```yaml
overload:
  enabled: false

phantom:
  address: cppserver:8080
  ammofile: /var/loadtest/ammo_local.txt
  ammo_type: uri

  load_profile:
    load_type: rps
    schedule: const(5, 1m)

  ssl: false

console:
  enabled: false

telegraf:
  enabled: false
```

---

### Назначение

Этот файл используется для обычного нагрузочного тестирования.

Нагрузка:

```text
5 RPS
1 минута
```

Модель:

```yaml
const(5, 1m)
```

означает:

```text
постоянные 5 запросов в секунду
в течение 1 минуты
```

Такой режим позволяет проверить выполнение требований по задержкам ответа:

```text
50-й персентиль ≤ 35 ms
90-й персентиль ≤ 50 ms
```

---

## 🚨 Файл load_stress.yaml

Содержимое:

```yaml
overload:
  enabled: false

phantom:
  address: cppserver:8080
  ammofile: /var/loadtest/ammo_local.txt
  ammo_type: uri

  load_profile:
    load_type: rps
    schedule: const(100000, 1m)

  ssl: false

console:
  enabled: false

telegraf:
  enabled: false
```

---

### Назначение

Этот файл используется для стресс-теста.

Нагрузка:

```text
100000 RPS
1 минута
```

Конфигурация:

```yaml
const(100000, 1m)
```

Позволяет проверить поведение сервера в экстремальных условиях.

---

## 📊 Используемые модели нагрузки

Постоянная нагрузка:

```yaml
const(5, 1m)
```

Линейный рост:

```yaml
line(5,30,1m)
```

Ступенчатая нагрузка:

```yaml
step(100,1000,100,30s)
```

В данной задаче используются только модели:

```text
const(5,1m)
const(100000,1m)
```

---

## 🖥️ Проверка файлов

Проверить содержимое:

```bash
cat ammo.txt
cat load.yaml
cat load_stress.yaml
```

Проверить структуру:

```bash
ls -la
```

Ожидаемый результат:

```text
README.md
ammo.txt
load.yaml
load_stress.yaml
```

---

## 🏁 Итог

Главная цепочка задания:

```text
ammo.txt
↓
load.yaml
↓
Yandex Tank
↓
Phantom
↓
HTTP запросы
↓
нагрузочное тестирование
```

Главная идея:

```text
Перед запуском нагрузочного теста необходимо подготовить корректный набор запросов и сценарии нагрузки для сервера.
```

---

## ⬅️ Назад

Вернуться к разделу:

```text
lessons/sprint_19_20/load_testing
```

или открыть:

```text
../../../lessons/sprint_19_20/load_testing/README.md
```
