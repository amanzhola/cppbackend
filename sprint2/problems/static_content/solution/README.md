# 🌐 Static Content Solution

---

## 🎯 Задача

| 📌 Что нужно было реализовать | 📖 Полное описание задачи                                                    | 🧠 Как это работает                                           | ✅ Итог                     |
| ----------------------------- | ---------------------------------------------------------------------------- | ------------------------------------------------------------- | -------------------------- |
| Отдача статических файлов     | Добавить в игровой сервер возможность отдавать статические файлы из каталога | Путь к каталогу передаётся вторым аргументом командной строки | Сервер поддерживает static |

---

## ▶️ Запуск по заданию

| 📌 Команда запуска                              | 📖 Аргумент        | 🧠 Что означает                    | ✅ Итог             |
| ----------------------------------------------- | ------------------ | ---------------------------------- | ------------------ |
| `build/bin/game_server data/config.json static` | `data/config.json` | путь к конфигурации игры           | Конфиг загружается |
|                                                 | `static`           | путь к каталогу статических файлов | Static подключён   |

```bash
build/bin/game_server data/config.json static
```

---

## 📂 Полный путь решения

```text
/home/ubuntu/cppbackend/sprint2/problems/static_content/solution
```

---

## 📁 Структура проекта

| 📁 Часть           | 📖 Что внутри      | 🧠 Назначение        | ✅ Итог                 |
| ------------------ | ------------------ | -------------------- | ---------------------- |
| `CMakeLists.txt`   | Сборка             | Конфигурация проекта | Сборка через CMake     |
| `Dockerfile`       | Docker             | Контейнеризация      | Готово для запуска     |
| `README.md`        | Документация       | Описание решения     | Документ есть          |
| `conanfile.txt`    | Зависимости        | Boost и др.          | Зависимости подключены |
| `data/config.json` | Конфиг             | Игровые данные       | Загружается            |
| `src/*`            | Исходный код       | Сервер и логика      | Backend реализован     |
| `static/*`         | HTML/CSS/JS/assets | Клиентская часть     | Frontend работает      |

```text
solution/
├── CMakeLists.txt
├── Dockerfile
├── README.md
├── conanfile.txt
├── data/
│   └── config.json
├── src/
│   ├── boost_json.cpp
│   ├── http_server.cpp
│   ├── http_server.h
│   ├── json_loader.cpp
│   ├── json_loader.h
│   ├── main.cpp
│   ├── model.cpp
│   ├── model.h
│   ├── request_handler.cpp
│   ├── request_handler.h
│   ├── sdk.h
│   └── tagged.h
└── static/
    ├── about.html
    ├── game.html
    ├── index.html
    ├── assets/
    ├── images/
    └── js/
```

---

## 🔀 Типы запросов

| 📌 Тип запроса | 📖 Что делает сервер        | 🧠 Пример      | ✅ Итог          |
| -------------- | --------------------------- | -------------- | --------------- |
| `/api/...`     | Обрабатывается как REST API | `/api/v1/maps` | API работает    |
| остальные      | Обрабатываются как static   | `/index.html`  | Static работает |

```text
/api/...      -> REST API
остальные     -> static
```

---

## 🔌 REST API

| 📌 Endpoint                 | 📖 Что возвращает | ✅ Итог |
| --------------------------- | ----------------- | ------ |
| `GET /api/v1/maps`          | список карт       | JSON   |
| `GET /api/v1/maps/{map_id}` | конкретная карта  | JSON   |

---

## 🌐 HTTP

| 📌 Термин | 📖 Расшифровка              | 🧠 Значение                        | ✅ Итог         |
| --------- | --------------------------- | ---------------------------------- | -------------- |
| HTTP      | HyperText Transfer Protocol | протокол общения клиента и сервера | Основа сервера |

```text
HyperText  -> гипертекст
Transfer   -> передача
Protocol   -> протокол
```

```http
GET /index.html HTTP/1.1
```

---

## 🔗 URI

| 📌 Термин | 📖 Расшифровка              | 🧠 Значение           | ✅ Итог              |
| --------- | --------------------------- | --------------------- | ------------------- |
| URI       | Uniform Resource Identifier | идентификатор ресурса | Указывает на ресурс |

```text
/index.html
/game.html
/js/game.js
/assets/road_vh.png
/api/v1/maps
```

---

## 🧠 REST API

| 📌 Термин | 📖 Расшифровка                      | 🧠 Значение        | ✅ Итог          |
| --------- | ----------------------------------- | ------------------ | --------------- |
| REST API  | Representational State Transfer API | передача состояния | Возвращает JSON |

```text
Representational -> представление
State            -> состояние
Transfer         -> передача
```

---

## 🧾 JSON

| 📌 Термин | 📖 Расшифровка             | 🧠 Значение   | ✅ Итог             |
| --------- | -------------------------- | ------------- | ------------------ |
| JSON      | JavaScript Object Notation | формат данных | Используется в API |

```http
Content-Type: application/json
```

---

## 📄 Static files

| 📌 Запрос             | 📖 Что происходит                      | ✅ Итог |
| --------------------- | -------------------------------------- | ------ |
| `/index.html`         | возвращает `static/index.html`         | HTML   |
| `/assets/road_vh.png` | возвращает `static/assets/road_vh.png` | PNG    |

---

## 🧾 Content-Type

| 📌 Термин    | 📖 Объяснение   | 🧠 Значение            | ✅ Итог |
| ------------ | --------------- | ---------------------- | ------ |
| Content-Type | MIME тип ответа | клиент понимает формат | Важен  |

---

## 🧾 MIME

| 📌 Термин | 📖 Расшифровка                        | 🧠 Значение     | ✅ Итог            |
| --------- | ------------------------------------- | --------------- | ----------------- |
| MIME      | Multipurpose Internet Mail Extensions | тип содержимого | Определяет формат |

---

## 📊 MIME-типы

| 📄 Расширение | 🌐 Content-Type          | ✅ Итог   |
| ------------- | ------------------------ | -------- |
| `.html`       | text/html                | HTML     |
| `.css`        | text/css                 | CSS      |
| `.txt`        | text/plain               | текст    |
| `.js`         | text/javascript          | JS       |
| `.json`       | application/json         | JSON     |
| `.xml`        | application/xml          | XML      |
| `.png`        | image/png                | PNG      |
| `.jpg`        | image/jpeg               | JPG      |
| `.gif`        | image/gif                | GIF      |
| `.bmp`        | image/bmp                | BMP      |
| `.ico`        | image/vnd.microsoft.icon | ICO      |
| `.tiff`       | image/tiff               | TIFF     |
| `.svg`        | image/svg+xml            | SVG      |
| `.mp3`        | audio/mpeg               | MP3      |
| unknown       | application/octet-stream | fallback |

---

## 📏 Content-Length

```text
Content-Length — размер тела ответа
```

---

## 🧠 HEAD

| 📌 Метод | 📖 Что делает               | ✅ Итог   |
| -------- | --------------------------- | -------- |
| HEAD     | возвращает только заголовки | без тела |

---

## 🔤 URL-encoding

| 📌 Термин    | 📖 Объяснение        | 🧠 Пример | ✅ Итог |
| ------------ | -------------------- | --------- | ------ |
| URL-encoding | кодирование символов | `%20`     | пробел |

```text
hello%20world.txt -> hello world.txt
```

---

## 🛡 Безопасность

| 📌 Тип         | 📖 Описание             | 🧠 Пример              | ✅ Итог    |
| -------------- | ----------------------- | ---------------------- | --------- |
| path traversal | попытка выйти из static | `/../data/config.json` | запрещено |

Ответ:

```http
400 Bad Request
Content-Type: text/plain
```

---

## ❌ Ошибки

| 📌 Ситуация     | 📖 Ответ        | ✅ Итог    |
| --------------- | --------------- | --------- |
| файл не найден  | 404 Not Found   | корректно |
| выход за static | 400 Bad Request | защита    |

---

## 🔧 Сборка

```bash
cd /home/ubuntu/cppbackend/sprint2/problems/static_content/solution

rm -rf build
mkdir build
cd build

cmake ..
cmake --build .
```

---

## ▶️ Запуск

```bash
./game_server ../data/config.json ../static
```

или

```bash
./bin/game_server ../data/config.json ../static
```

Ожидаемый вывод:

```text
Server has started...
```

---

## 🧪 Проверка через WSL

```bash
curl -i http://localhost:8080/api/v1/maps
curl -i http://localhost:8080/
curl -i http://localhost:8080/index.html
curl -i http://localhost:8080/about.html
curl -i http://localhost:8080/game.html
curl -i http://localhost:8080/js/game.js
curl -i http://localhost:8080/assets/road_vh.png -o /dev/null -D -
curl -i http://localhost:8080/images/cube.svg
curl -I http://localhost:8080/index.html
curl -i --path-as-is http://localhost:8080/../data/config.json
```

---

## ✅ Ожидаемые результаты

```text
/api/v1/maps         -> 200 OK
/                    -> 200 OK
/index.html          -> 200 OK
/about.html          -> 200 OK
/game.html           -> 200 OK
/js/game.js          -> 200 OK
/assets/road_vh.png  -> 200 OK
/images/cube.svg     -> 200 OK
HEAD /index.html     -> 200 OK (без тела)
/../data/config.json -> 400 Bad Request
```

---

## 🌍 Проверка через браузер

Открыть:

```text
http://localhost:8080/
```

Проверить:

```text
http://localhost:8080/game.html
http://localhost:8080/api/v1/maps
```

---

## 🏁 Итог

| 📌 Что реализовано | 📖 Результат   | ✅ Итог   |
| ------------------ | -------------- | -------- |
| Static server      | HTML/CSS/JS    | работает |
| REST API           | JSON           | работает |
| MIME               | корректный     | работает |
| безопасность       | path traversal | защищено |

```text
браузер открывает клиентскую часть игры
```
