# 🌐 HTTP Server Lesson 9_4 (accept + write)

---

## 📌 Описание

В этом задании реализован минимальный HTTP-сервер на Boost.Asio, который:

* слушает порт 8080
* принимает подключение (`accept`)
* отправляет HTTP-ответ (`write`)
* возвращает клиенту текст:

```text
Hello
```

---

## 🎯 Цель

Понять базовую механику веб-сервера:

* `accept` → принять подключение
* `write` → отправить HTTP-ответ
* `Content-Type` → объяснить браузеру формат
* пустая строка → разделить заголовки и тело

---

# 🧠 Код ответа сервера

```cpp
constexpr std::string_view response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello";
```

---

# ⚙️ Сборка (WSL + CMake + Conan)

```bash
mkdir build
cd build
conan install .. --build=missing
cmake ..
cmake --build .
```

---

# ▶️ Запуск

```bash
./bin/server
```

Вывод:

```text
Waiting for socket connection
```

---

# 🌐 Проверка через браузер

Открыть:

```text
http://127.0.0.1:8080
```

---

## 📥 Результат

### Сервер:

```text
Waiting for socket connection
Connection received
```

### Браузер:

```text
Hello
```

---

# 🔍 Проверка через curl

```bash
curl -v http://127.0.0.1:8080
```

---

## 📥 Результат curl

```text
> GET / HTTP/1.1
> Host: 127.0.0.1:8080

< HTTP/1.1 200 OK
< Content-Type: text/plain

Hello
```

---

## ⚠️ Важно

`curl` может показать:

```text
Recv failure: Connection reset by peer
```

👉 Это нормально.

Причина:

* сервер не отправляет `Content-Length`
* клиент ждёт завершение ответа по закрытию соединения
* соединение закрывается → `curl` ругается

---

# 🔥 Проверка через DevTools (F12)

## 📌 Полная последовательность

### Шаг 1

Запусти сервер:

```bash
./bin/server
```

---

### Шаг 2

Открой браузер:

```text
http://127.0.0.1:8080
```

---

### Шаг 3

Открой DevTools:

```text
F12
```

---

### Шаг 4

Перейди во вкладку:

```text
Network
```

---

### ⚠️ Важно

Если список пуст — это нормально.

---

### Шаг 5

Обнови страницу:

```text
F5
```

---

### Шаг 6

Слева появится запрос:

```text
127.0.0.1
```

---

### Шаг 7

Кликни на него

---

### Шаг 8

Смотри вкладки справа:

```text
Headers | Preview | Response
```

---

# 🔍 Что ты увидишь

## 📌 Headers

### Request

```text
Request Method: GET
```

👉 Браузер показывает метод отдельно
(полная строка на уровне HTTP: `GET / HTTP/1.1`)

---

### Response Headers

```text
Status Code: 200 OK
Content-Type: text/plain
```

---

## 📌 Response

```text
Hello
```

---

## 📌 Preview

```text
Hello
```

---

# 🧠 Почему DevTools важен

Без DevTools ты видишь только:

```text
Hello
```

С DevTools ты видишь:

```text
✔ Request Method: GET
✔ Status Code: 200 OK
✔ Content-Type
✔ тело ответа
```

👉 То есть весь HTTP-обмен

---

# ⚠️ Частая ошибка

❌ Открыть DevTools после загрузки страницы
→ ничего не видно

✅ Правильно:

```text
F12 → Network → F5
```

---

# 💡 Дополнительно

## favicon.ico

Браузер может отправить второй запрос:

```text
favicon.ico
```

👉 Это нормально — попытка загрузить иконку сайта

---

# 🧠 Что изменилось по сравнению с lesson9_3

| Версия    | Поведение              |
| --------- | ---------------------- |
| lesson9_3 | только accept          |
| lesson9_4 | accept + HTTP response |

---

# ⚠️ Ограничения сервера

Сервер:

* принимает только одно соединение
* не читает запрос полностью
* не работает в цикле
* не обрабатывает несколько клиентов

---

# 🧠 Главная идея

```text
accept → подключение
write → HTTP-ответ
```

---

# 🏁 Итог

Теперь сервер:

```text
✔ принимает соединение
✔ отправляет HTTP-ответ
✔ браузер корректно отображает результат
```

---

## 🚀 Короткая формула

```text
F12 → Network → F5 → клик → Headers + Response
```

---

Это уже минимальный, но настоящий HTTP-сервер 🌐
