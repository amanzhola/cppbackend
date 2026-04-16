# 🌐 HTTP Server Lesson 10_2 (JSON + HTML)

---

## 🚀 Обзор

| 📌 Параметр | 💡 Значение        |
| ----------- | ------------------ |
| Тип сервера | HTTP (Boost.Beast) |
| Форматы     | JSON + HTML        |
| Протокол    | TCP                |
| Порт        | 8080               |
| Методы      | GET / POST         |

---

## 🧠 Возможности

| 🌐 Routes | 📦 Формат | 🔁 Поведение |
| --------- | --------- | ------------ |
| `/`       | JSON      | Main page    |
| `/hello`  | JSON      | Hello        |
| `/bye`    | JSON      | Goodbye      |
| `/echo`   | JSON      | echo body    |
| `/html`   | HTML      | страница     |
| unknown   | JSON      | error        |

---

# 🌐 Маршруты

| Метод | URL      | Ответ                     | Тип  |
| ----- | -------- | ------------------------- | ---- |
| GET   | `/`      | `{"message":"Main page"}` | JSON |
| GET   | `/hello` | `{"message":"Hello!"}`    | JSON |
| GET   | `/bye`   | `{"message":"Goodbye!"}`  | JSON |
| POST  | `/echo`  | `{"echo":"..."}`          | JSON |
| GET   | `/html`  | HTML страница             | HTML |
| ANY   | unknown  | `{"error":"Not found"}`   | JSON |

---

# 🔍 Проверка и результат

| 🔍 Проверка (curl)                                                                                                                                | 📥 Результат                                                                                                 |
| ------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------ |
| `bash<br>curl http://localhost:8080/<br>curl http://localhost:8080/hello<br>curl http://localhost:8080/bye<br>curl http://localhost:8080/unknown` | `json<br>{"message":"Main page"}<br>{"message":"Hello!"}<br>{"message":"Goodbye!"}<br>{"error":"Not found"}` |

---

# 🔁 Echo и HTML

| 🔍 Запрос                                                                | 📥 Ответ                                                           |
| ------------------------------------------------------------------------ | ------------------------------------------------------------------ |
| `bash<br>curl -X POST http://localhost:8080/echo -d "Hello from client"` | `json<br>{"echo":"Hello from client"}`                             |
| `bash<br>curl http://localhost:8080/html`                                | `html<br><!doctype html><html>...<h1>Hello from HTML page</h1>...` |

---

# 🖥 Логи и HTTP

| 🖥 Логи сервера                                                                                                                            | 🌐 HTTP Ответ                                                                             |
| ------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------- |
| `text<br>Connection received<br>GET /<br>Connection received<br>POST /echo<br>Body: Hello from client<br>Connection received<br>GET /html` | `http<br>HTTP/1.1 200 OK<br>Content-Type: text/html<br><br><h1>Hello from HTML page</h1>` |

---

# ⚙️ Сборка

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

---

# 🌐 Проверка в браузере

Открой:

```text
http://localhost:8080/html
```

👉 увидишь страницу:

```text
Hello from HTML page
```

---

# 🧩 Логика обработки

| Шаг | Действие                    |
| --- | --------------------------- |
| 1   | accept соединение           |
| 2   | read HTTP request           |
| 3   | проверка method             |
| 4   | проверка target             |
| 5   | выбор формата (JSON / HTML) |
| 6   | write response              |

---

# 🔥 Что нового

| Версия        | Возможность |
| ------------- | ----------- |
| lesson_10_0   | маршруты    |
| lesson_10_0_1 | JSON        |
| lesson_10_1   | POST + body |
| lesson_10_2   | HTML + JSON |

---

# ⚠️ Важно

| Проблема         | Причина               | Решение           |
| ---------------- | --------------------- | ----------------- |
| JSON не читается | неверный Content-Type | application/json  |
| HTML как текст   | text/plain            | text/html         |
| echo не работает | не POST               | использовать POST |

---

# 🧠 Архитектура

```text
Client (curl / browser)
        ↓
    TCP socket
        ↓
    HTTP request
        ↓
    routing (method + target)
        ↓
    JSON / HTML response
```

---

# 🧪 Примеры

| Команда       | Результат |
| ------------- | --------- |
| `curl /`      | JSON      |
| `curl /hello` | JSON      |
| `curl /html`  | HTML      |
| `POST /echo`  | echo JSON |

---

# 🏁 Итог

| ✔ Что сделано        |
| -------------------- |
| маршруты             |
| JSON API             |
| POST обработка       |
| HTML ответ           |
| универсальный сервер |

---

## 🚀 Формула

```text
method + target → выбор формата → HTTP response
```

---

🔥 Это уже почти полноценный веб-сервер
