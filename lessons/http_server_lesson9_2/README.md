# HTTP Server Lesson 9_2 (мини-упражнения)

---

## 📌 Описание

В этой версии проекта были выполнены дополнительные мини-упражнения для закрепления работы простого HTTP-сервера на Boost.Asio.

Работа выполнялась в **WSL (Ubuntu)**.
Сборка выполнялась через **CMake + Conan**.

Эта версия основана на предыдущем проекте `http_server_lesson9`, но дополнительно показывает:

* как изменить тело HTTP-ответа
* как заменить обычный текст на HTML
* как сравнить HTTP-запросы от `curl` и браузера

---

## 🧠 Цель

Закрепить понимание того, что HTTP-ответ состоит из:

* статусной строки
* заголовков
* пустой строки
* тела ответа

И увидеть на практике, что:

* `Content-Length` должен соответствовать содержимому
* `Content-Type` влияет на поведение клиента
* разные клиенты отправляют разные заголовки

---

## ⚙️ Среда выполнения

* ОС: WSL (Ubuntu)
* Компилятор: g++
* Система сборки: CMake
* Менеджер зависимостей: Conan
* Библиотека: Boost.Asio

---

# 1. Создание проекта

Для этой версии была создана отдельная папка проекта:

```bash
cd ~/cppbackend/lessons
cp -r http_server_lesson9 http_server_lesson9_3
cd http_server_lesson9_3
rm -rf build
```

Таким образом, за основу была взята рабочая версия `http_server_lesson9`, после чего код был изменён для выполнения дополнительных упражнений.

---

# 2. Обновлённый main.cpp

В файле `main.cpp` был использован следующий код:

```cpp
#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <array>
#include <string_view>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

int main() {
    try {
        // Контекст для операций ввода-вывода
        net::io_context ioc;

        // Слушаем все IPv4-интерфейсы на порту 8080
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "HTTP server started on port 8080"sv << std::endl;
        std::cout << "Waiting for socket connection..."sv << std::endl;

        // Сокет для общения с клиентом
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        std::cout << "Connection received"sv << std::endl;

        // Буфер для чтения запроса клиента
        std::array<char, 4096> buffer{};
        const std::size_t bytes_read = socket.read_some(net::buffer(buffer));

        std::cout << "Request received:"sv << std::endl;
        std::cout.write(buffer.data(), static_cast<std::streamsize>(bytes_read));
        std::cout << std::endl;

        // HTTP-ответ с HTML-страницей
        const std::string_view response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: 53\r\n"
            "\r\n"
            "<html><body><h1>Hello, Web-server!</h1></body></html>";

        net::write(socket, net::buffer(response));

        std::cout << "Response sent"sv << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: "sv << e.what() << std::endl;
        return 1;
    }
}
```

---

# 3. Мини-упражнение 1: изменить тело ответа

Исходно сервер возвращал:

```text
Hello world!
```

В упражнении тело ответа было изменено на:

```text
Hello, Web-server!
```

---

## Почему нужно менять Content-Length

Если тело ответа меняется, необходимо изменить и заголовок:

```http
Content-Length: ...
```

Потому что клиент должен точно знать, сколько байт содержится в теле ответа.

---

## Как посчитать длину

Строка:

```text
Hello, Web-server!
```

содержит 18 символов:

* `Hello` = 5
* `,` = 1
* пробел = 1
* `Web-server` = 10
* `!` = 1

Итого:

```text
18
```

Поэтому для текстового ответа нужно было бы указать:

```http
Content-Length: 18
```

---

## Пример текстового ответа

```cpp
const std::string_view response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain; charset=UTF-8\r\n"
    "Content-Length: 18\r\n"
    "\r\n"
    "Hello, Web-server!";
```

---

# 4. Мини-упражнение 2: вернуть HTML вместо текста

Во втором упражнении обычный текстовый ответ был заменён на HTML-страницу.

Использованный ответ:

```cpp
const std::string_view response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 53\r\n"
    "\r\n"
    "<html><body><h1>Hello, Web-server!</h1></body></html>";
```

---

## Что изменилось

### Было:

* `Content-Type: text/plain`
* клиент получал обычный текст

### Стало:

* `Content-Type: text/html`
* браузер интерпретирует тело ответа как HTML-документ

---

## Почему Content-Length = 53

Тело ответа:

```html
<html><body><h1>Hello, Web-server!</h1></body></html>
```

содержит 53 символа.

Поэтому был указан заголовок:

```http
Content-Length: 53
```

---

## Результат в браузере

После запуска сервера и открытия адреса:

```text
http://127.0.0.1:8080/
```

браузер отобразил HTML-страницу с заголовком:

```text
Hello, Web-server!
```

---

# 5. Сборка проекта

После изменения `main.cpp` проект был пересобран:

```bash
mkdir build
cd build
conan install .. --build=missing
cmake ..
cmake --build .
```

---

# 6. Запуск сервера

```bash
./bin/server
```

Ожидаемый вывод:

```text
HTTP server started on port 8080
Waiting for socket connection...
```

---

# 7. Проверка через браузер

После запуска сервера в браузере был открыт адрес:

```text
http://127.0.0.1:8080/
```

Результат:

* браузер успешно подключился к серверу
* получил HTML-ответ
* отобразил страницу с заголовком `Hello, Web-server!`

---

# 8. Мини-упражнение 3: сравнить запрос от curl и от браузера

## Проверка через curl

Во втором терминале WSL:

```bash
curl -v http://127.0.0.1:8080/
```

---

## Что обычно показывает curl

Типичный запрос от `curl` выглядит примерно так:

```http
GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: curl/8.5.0
Accept: */*
```

Это короткий и простой HTTP-запрос.

---

## Проверка через браузер

При открытии страницы в браузере сервер получает гораздо больше заголовков.

Пример запроса от браузера:

```http
GET / HTTP/1.1
Host: 127.0.0.1:8080
Connection: keep-alive
sec-ch-ua: "Chromium";v="146", "Not-A.Brand";v="24", "Google Chrome";v="146"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Windows"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: en-US,en;q=0.9,ru;q=0.8
```

---

## Что можно увидеть при сравнении

При сравнении запросов от `curl` и браузера видно, что:

* заголовки отличаются
* `User-Agent` отличается
* браузер отправляет намного больше заголовков
* браузер сообщает клиентскую платформу, язык, предпочтения форматов и режим загрузки страницы

---

## Почему это полезно

Это важно для понимания реального HTTP:

* разные клиенты ведут себя по-разному
* сервер должен быть готов к разным наборам заголовков
* браузер и консольные клиенты используют один и тот же протокол, но по-разному формируют запросы

---

# 9. Что показывает это упражнение

Это мини-упражнение помогает увидеть на практике:

1. что HTTP-ответ — это обычный текстовый протокол
2. что заголовки ответа имеют значение
3. что `Content-Type` определяет способ обработки ответа
4. что `Content-Length` должен быть точным
5. что разные клиенты отправляют разные HTTP-запросы

---

# 10. Ограничения сервера

Как и базовая версия, этот сервер остаётся учебным примером.

Он:

* принимает только одно соединение
* читает только один запрос
* отправляет только один ответ
* завершает работу после обработки запроса

---

# 11. Итог

В рамках версии `http_server_lesson9_3` были выполнены три мини-упражнения:

1. изменено тело ответа с `Hello world!` на `Hello, Web-server!`
2. текстовый ответ заменён на HTML-страницу
3. выполнено сравнение HTTP-запросов от `curl` и браузера

Результат:

* сервер успешно возвращает изменённый ответ
* браузер корректно отображает HTML
* было на практике показано различие между клиентами `curl` и браузером

Эта версия помогает лучше понять устройство HTTP-ответов и поведение разных HTTP-клиентов.
