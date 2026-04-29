# Sprint 18_20 — Theme 1_4 — Lesson 3_10 — Static Files Part 5

---

## 🧩 Тема

| 🧩 Тема практического урока                            | 📖 Полное описание темы                                              | 🧠 Что изучается                                                               | ✅ Итог                                       |
| ------------------------------------------------------ | -------------------------------------------------------------------- | ------------------------------------------------------------------------------ | -------------------------------------------- |
| Поддержка файлов в Boost.Beast через `http::file_body` | Поддержка файлов в Boost.Beast через `boost::beast::http::file_body` | Переход от чтения файлов в `std::string` к потоковой отдаче файлов через Beast | Сервер приближается к реальному HTTP-серверу |

---

## 📂 Полный путь проекта

| 📂 Что указывается | 📖 Полный путь проекта                                                                  | 🧠 Зачем нужен путь               | ✅ Итог            |
| ------------------ | --------------------------------------------------------------------------------------- | --------------------------------- | ----------------- |
| Папка проекта      | `/home/ubuntu/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_3_10_static_files_part5` | Здесь находится весь проект part5 | Путь зафиксирован |

```text
/home/ubuntu/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_3_10_static_files_part5
```

---

## 🎯 Цель практики

| 🎯 Цель               | 📖 Что изменилось                                                     | 🧠 Почему это важно                                  | ✅ Итог                              |
| --------------------- | --------------------------------------------------------------------- | ---------------------------------------------------- | ----------------------------------- |
| Перевод отдачи файлов | Сервер переведён с чтения файлов в `std::string` на `http::file_body` | Убирается необходимость загружать весь файл в память | Отдача файлов становится правильной |

```cpp
boost::beast::http::file_body
```

---

## 📁 Структура проекта

| 📁 Часть         | 📖 Что внутри        | 🧠 Назначение         | ✅ Итог               |
| ---------------- | -------------------- | --------------------- | -------------------- |
| `CMakeLists.txt` | Файл сборки          | Конфигурация проекта  | Сборка через CMake   |
| `README.md`      | Документация         | Описание урока        | Документ готов       |
| `secret.txt`     | Файл вне static      | Проверка безопасности | Не должен отдаваться |
| `build/`         | Папка сборки         | Бинарники             | Отделена от src      |
| `src/main.cpp`   | Сервер               | Вся логика HTTP       | Сервер работает      |
| `static/*`       | HTML, CSS, JS, media | Клиентская часть      | Static работает      |

```text
sprint_18_20_theme_1_4_lesson_3_10_static_files_part5/
├── CMakeLists.txt
├── README.md
├── secret.txt
├── build/
├── src/
│   └── main.cpp
└── static/
    ├── app.js
    ├── file..name.txt
    ├── hello world.txt
    ├── index.html
    ├── page with space.html
    ├── style.css
    ├── audio/
    │   └── front_center.wav
    ├── images/
    │   ├── road_t.png
    │   ├── road_tr.png
    │   ├── road_vh.png
    │   └── tv_architecture.png
    └── video/
        ├── reset_position.mp4
        ├── track_horizontal_plane.mp4
        └── track_vertical_plane.mp4
```

---

## 🧠 Теория: Boost

| 🧠 Термин | 📖 Объяснение           | 🧠 Значение                         | ✅ Итог              |
| --------- | ----------------------- | ----------------------------------- | ------------------- |
| Boost     | Набор библиотек для C++ | Многие идеи позже входят в стандарт | Используется широко |

---

## 🧠 Boost.Asio

| 🧠 Термин | 📖 Расшифровка            | 🧠 Значение            | ✅ Итог               |
| --------- | ------------------------- | ---------------------- | -------------------- |
| Asio      | Asynchronous Input/Output | Асинхронный ввод/вывод | Основа сетевого кода |

```text
Asynchronous → асинхронный
Input        → ввод
Output       → вывод
```

---

## 🧠 Boost.Beast

| 🧠 Термин   | 📖 Объяснение               | 🧠 Значение          | ✅ Итог                |
| ----------- | --------------------------- | -------------------- | --------------------- |
| Boost.Beast | HTTP и WebSocket библиотека | Работает поверх Asio | Используется сервером |

---

## 🧠 TCP

| 🧠 Термин | 📖 Расшифровка                | 🧠 Значение       | ✅ Итог      |
| --------- | ----------------------------- | ----------------- | ----------- |
| TCP       | Transmission Control Protocol | Надёжная передача | Основа HTTP |

---

## 🧠 HTTP response

| 🧠 Часть    | 📖 Описание | ✅ Итог       |
| ----------- | ----------- | ------------ |
| status line | статус      | OK / error   |
| headers     | заголовки   | Content-Type |
| body        | тело        | данные       |

```http
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 11

Hello world
```

---

## ⚠️ string_body

| 📌 Что это    | 📖 Объяснение        | ❌ Минус                 |
| ------------- | -------------------- | ----------------------- |
| `string_body` | тело в `std::string` | файл полностью в памяти |

```cpp
http::response<http::string_body>
```

---

## ✅ file_body

| 📌 Что это  | 📖 Объяснение         | ✅ Плюсы                 |
| ----------- | --------------------- | ----------------------- |
| `file_body` | тело связано с файлом | не грузит файл в память |

```cpp
http::response<http::file_body>
```

Плюсы:

```text
не надо читать файл вручную
подходит для больших файлов
Content-Length ставится автоматически
```

---

## 🧠 file_body::value_type

```cpp
http::file_body::value_type file;
```

```cpp
file.open(file_path.c_str(), beast::file_mode::read, ec);
```

---

## 🧠 error_code

```cpp
boost::system::error_code
```

Позволяет обрабатывать ошибки без исключений.

---

## 🧠 prepare_payload

```cpp
response.prepare_payload();
```

Выставляет:

```text
Content-Length
Transfer-Encoding
```

---

## 🧠 Content-Length

```http
Content-Length: 240740
```

Размер ответа в байтах.

---

## 🔄 Главное изменение

| 🔄 Было       | 📖 Стало    |
| ------------- | ----------- |
| `std::string` | `file_body` |

```cpp
std::string body = ReadFile(...);
```

↓

```cpp
http::file_body::value_type file;
```

---

## 🚀 Почему это лучше

| 📌 Тип файлов | 📖 Почему важно  |
| ------------- | ---------------- |
| video/mp4     | большой размер   |
| audio/wav     | потоковые данные |
| images        | бинарные данные  |

`file_body` не грузит всё в память.

---

## 🛠 Что реализовано

```text
1. file_body
2. IsSubPath
3. UrlDecode
4. API разделение
5. MIME
6. HTML/CSS/JS
7. images
8. audio
9. video
10. безопасность
```

---

## 🔧 Сборка

```bash
cd /home/ubuntu/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_3_10_static_files_part5

rm -rf build
mkdir build
cd build

cmake ..
cmake --build .
```

---

## ▶️ Запуск

```bash
./static_server
```

```text
Server started: http://localhost:8080
Static root: "/home/ubuntu/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_3_10_static_files_part5/static"
```

---

## 🧪 Проверка

```bash
curl -i http://localhost:8080/
curl -i http://localhost:8080/file..name.txt
curl -i http://localhost:8080/images/road_vh.png -o /dev/null -D -
curl -i http://localhost:8080/audio/front_center.wav -o /dev/null -D -
curl -i http://localhost:8080/video/reset_position.mp4 -o /dev/null -D -
curl -i http://localhost:8080/api/v1/maps
curl -i --path-as-is http://localhost:8080/%2E%2E%2Fsecret.txt
```

```text
/ → 200 OK
/file..name.txt → 200 OK
/images → 200 OK
/audio → 200 OK
/video → 200 OK
/api → 200 OK
/secret → 403 Forbidden
```

---

## 🌍 Браузер

```text
http://localhost:8080/
```

Проверить:

```text
1. HTML
2. картинки
3. звук
4. видео
5. API
```

---

## 🏁 Итог

```text
сервер больше не читает файлы в std::string
```

```cpp
boost::beast::http::file_body
```

Это:

```text
быстрее
правильнее
ближе к production
```
