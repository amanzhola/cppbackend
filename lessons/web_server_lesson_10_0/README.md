# 🌐 HTTP Server Lesson 10_0 (маршруты)

---

## 📌 Описание

В этом задании реализован HTTP-сервер на Boost.Beast с поддержкой маршрутов.

Теперь сервер:

* принимает HTTP-запросы
* читает `target()`
* возвращает разные ответы в зависимости от пути

---

## 🎯 Цель

Изучить:

* разветвление логики
* работу с `req.target()`
* HTTP-статусы
* формирование ответа

---

# 🧠 Основная идея

```text
GET /        → Main page
GET /hello   → Hello!
GET /bye     → Goodbye!
GET unknown  → 404
```

---

# 🧱 Код маршрутизации

```cpp
if (req.method() != http::verb::get) {
    res.result(http::status::method_not_allowed);
    res.body() = "Only GET is supported\n";
} else if (req.target() == "/") {
    res.result(http::status::ok);
    res.body() = "Main page\n";
} else if (req.target() == "/hello") {
    res.result(http::status::ok);
    res.body() = "Hello!\n";
} else if (req.target() == "/bye") {
    res.result(http::status::ok);
    res.body() = "Goodbye!\n";
} else {
    res.result(http::status::not_found);
    res.body() = "404\n";
}
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

# ▶️ Запуск сервера

```bash
./bin/server
```

Вывод:

```text
Server started on port 8080
Waiting for connection...
```

---

# 🔍 Проверка через curl

Во втором терминале:

```bash
curl http://localhost:8080/
curl http://localhost:8080/hello
curl http://localhost:8080/bye
curl http://localhost:8080/unknown
```

---

## 📥 Результат

```text
Main page
Hello!
Goodbye!
404
```

---

# 🖥 Логи сервера

```text
Connection received
GET /
Connection received
GET /hello
Connection received
GET /bye
Connection received
GET /unknown
```

---

# 🧠 Что происходит внутри

## При каждом запросе:

1. клиент подключается
2. сервер читает HTTP-запрос
3. извлекает:

   ```text
   req.method()
   req.target()
   ```
4. вызывает `make_response(...)`
5. отправляет ответ

---

# 🌐 Пример HTTP-ответа

```http
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: ...

Hello!
```

---

# ⚠️ Важный момент (перевод строки)

Ответы содержат `\n`:

```cpp
res.body() = "Hello!\n";
```

👉 Это нужно, чтобы `curl` выводил результат красиво:

```text
Hello!
ubuntu@...
```

Без `\n` текст "прилипает" к приглашению терминала.

---

# 🔥 Что нового по сравнению с lesson9

| Версия   | Возможности              |
| -------- | ------------------------ |
| lesson9  | один фиксированный ответ |
| lesson10 | маршрутизация по URL     |

---

# 📡 Архитектура

```text
Client (curl / browser)
        ↓
    TCP socket
        ↓
    HTTP request
        ↓
    make_response()
        ↓
    HTTP response
```

---

# 🧪 Примеры запросов

```bash
curl http://localhost:8080/
→ Main page

curl http://localhost:8080/hello
→ Hello!

curl http://localhost:8080/bye
→ Goodbye!

curl http://localhost:8080/unknown
→ 404
```

---

# 🧠 Что ты изучил

* HTTP маршрутизацию
* обработку разных путей
* статус-коды
* структуру HTTP-ответа
* работу Boost.Beast

---

# ⚠️ Ограничения

Сервер:

* синхронный
* обрабатывает по одному соединению
* не держит keep-alive
* не масштабируется

---

# 🏁 Итог

```text
✔ чтение HTTP-запроса
✔ анализ пути
✔ разные ответы
✔ статус-коды
✔ рабочий веб-сервер
```

---

## 🚀 Короткая формула

```text
req.target() → выбор маршрута → HTTP response
```

---

Это уже настоящий backend-базис 🌐
