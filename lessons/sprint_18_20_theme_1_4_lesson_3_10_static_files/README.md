# 🌐 Урок: Отдача статических файлов

---

# 🎯 Цель урока

| 📌 Что изучаем            | 📖 Полное описание                                                              | 🧠 Что это значит                                        | ✅ Итог                            |
| ------------------------- | ------------------------------------------------------------------------------- | -------------------------------------------------------- | --------------------------------- |
| Отдача статических файлов | HTTP-сервер начинает отдавать HTML, CSS, JavaScript, изображения и другие файлы | Сервер становится не только API, но и сервером фронтенда | Можно открыть страницу в браузере |
| Игровой клиент            | index.html, style.css, app.js, images                                           | Браузер получает ресурсы с сервера                       | Полноценная клиентская часть      |
| URL                       | `http://localhost:8080/`                                                        | Запрос к серверу                                         | Загружается HTML                  |

---

# 📂 Что такое статические файлы

| 📌 Тип      | 📖 Описание | 🧠 Пример  | ✅ Итог      |
| ----------- | ----------- | ---------- | ----------- |
| HTML        | Страница    | index.html | Структура   |
| CSS         | Стили       | style.css  | Внешний вид |
| JS          | Скрипты     | app.js     | Логика      |
| Изображения | Медиа       | logo.png   | Ресурсы     |

```text
GET /index.html → static/index.html
```

---

# 🌍 Что такое HTTP

| 📌 Элемент | 📖 Значение     |
| ---------- | --------------- |
| GET        | получить ресурс |
| /style.css | путь            |
| HTTP/1.1   | версия          |

```http
GET /style.css HTTP/1.1
Host: localhost:8080
```

---

# 🔌 REST API

| 📌 Что   | 📖 Описание   |
| -------- | ------------- |
| REST API | отдаёт данные |
| JSON     | формат        |

```json
[
  { "id": "map1", "name": "Desert" }
]
```

👉 Но браузеру нужен ещё HTML + CSS + JS

---

# 🧱 HTML

| 📌 Что | 📖 Описание        |
| ------ | ------------------ |
| HTML   | структура страницы |

```text
static/index.html
```

---

# 🎨 CSS

| 📌 Что | 📖 Описание |
| ------ | ----------- |
| CSS    | внешний вид |

```text
static/style.css
```

---

# ⚡ JavaScript

| 📌 Что | 📖 Описание |
| ------ | ----------- |
| JS     | логика      |

```text
static/app.js
```

---

# 🧾 MIME type

| Расширение | Content-Type           |
| ---------- | ---------------------- |
| .html      | text/html              |
| .css       | text/css               |
| .js        | application/javascript |
| .json      | application/json       |
| .png       | image/png              |

👉 Определяет, как браузер обрабатывает ответ

---

# 🎯 Практическая цель

```text
GET /              → index.html
GET /style.css     → style.css
GET /app.js        → app.js
GET /unknown       → 404
GET /../../secret  → 403
```

---

# 📂 Структура проекта

```text
sprint_18_20_theme_1_4_lesson_3_10_static_files/
├── CMakeLists.txt
├── README.md
├── build/
├── src/main.cpp
└── static/
    ├── index.html
    ├── style.css
    └── app.js
```

---

# ⚠️ Проблема с Boost

| 📌 Проблема     | 📖 Причина               | ✅ Решение          |
| --------------- | ------------------------ | ------------------ |
| Boost не найден | нет `/usr/include/boost` | использовать Conan |
| Уже есть Boost  | ~/.conan/...             | использовать его   |
| Не ставим apt   | экономим 500MB           | ✔                  |

---

# ⚙️ CMake

| Команда             | Значение     |
| ------------------- | ------------ |
| project             | проект       |
| add_executable      | бинарник     |
| include_directories | путь к Boost |
| pthread             | потоки       |

---

# 📄 HTML

```html
<link rel="stylesheet" href="/style.css">
<script src="/app.js"></script>
```

👉 браузер делает отдельные запросы

---

# 🎨 CSS

```css
background-color: #202124;
```

---

# ⚡ JS

```javascript
context.fillRect(100, 100, 80, 80);
```

---

# ❌ Ошибка с путём

| 📌 Было      | ❌ Неправильно |
| ------------ | ------------- |
| build/static | нет файлов    |

| 📌 Стало  | ✅ Правильно   |
| --------- | ------------- |
| ../static | файлы найдены |

```cpp
fs::current_path().parent_path() / "static";
```

---

# 🧠 Логика сервера

| Запрос     | Ответ      |
| ---------- | ---------- |
| /          | index.html |
| /style.css | CSS        |
| /app.js    | JS         |
| unknown    | 404        |
| ../../     | 403        |

---

# 🔧 Сборка

```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

---

# ▶️ Запуск

```bash
./static_server
```

---

# 🧪 Тесты

```bash
curl -i http://localhost:8080/
curl -i http://localhost:8080/style.css
curl -i http://localhost:8080/app.js
curl -i http://localhost:8080/unknown
```

---

# 🌐 Браузер

```text
http://localhost:8080/
```

👉 откроется страница

---

# 💻 WSL

| 📌 Что    | 📖      |
| --------- | ------- |
| сервер    | Linux   |
| браузер   | Windows |
| localhost | общий   |

---

# 🔢 HTTP коды

| Код | Значение           |
| --- | ------------------ |
| 200 | OK                 |
| 403 | Forbidden          |
| 404 | Not Found          |
| 405 | Method Not Allowed |

---

# 🛡 Path Traversal

```text
/../../secret
```

👉 защита:

```cpp
IsSubPath(...)
```

---

# 🏁 Итог

| 📌 Сделано        | ✅ |
| ----------------- | - |
| HTTP сервер       | ✔ |
| статические файлы | ✔ |
| MIME              | ✔ |
| безопасность      | ✔ |
| исправлен путь    | ✔ |
| Boost через Conan | ✔ |
| тесты curl        | ✔ |
| браузер           | ✔ |

---

👉 Главный результат:

```text
http://localhost:8080/
```

✔ открывает HTML
✔ грузит CSS
✔ грузит JS
✔ рисует canvas
