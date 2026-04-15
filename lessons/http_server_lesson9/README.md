# HTTP Server Lesson 9 (WSL, CMake + Conan)

---

## 📌 Описание

В этом задании был реализован минимальный HTTP-сервер на C++ с использованием Boost.Asio.

Работа выполнялась в среде **WSL (Ubuntu)**.
Сборка выполнялась с использованием **CMake + Conan**.

Сервер:

* принимает одно TCP-соединение
* читает HTTP-запрос
* отправляет фиксированный HTTP-ответ
* завершает работу

---

## 🧠 Цель задания

Цель — понять базовую механику HTTP и сетевого взаимодействия:

* установление TCP-соединения
* отправка HTTP-запроса клиентом
* чтение запроса сервером
* формирование HTTP-ответа
* отправка ответа клиенту

---

## ⚙️ Среда выполнения

* ОС: WSL (Ubuntu)
* Компилятор: g++
* Система сборки: CMake
* Менеджер зависимостей: Conan
* Библиотека: Boost.Asio

---

# 1. Практическое задание: создать файл

## Переход в каталог проекта

```bash
cd ~/cppbackend/lessons
pwd
ls
```

## Создание каталога проекта

```bash
mkdir http_server_lesson9
cd http_server_lesson9
pwd
```

Ожидаемый путь:

```text
/home/ubuntu/cppbackend/lessons/http_server_lesson9
```

---

## Создание файла main.cpp

```bash
nano main.cpp
```

Вставить следующий код:

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

        // Фиксированный HTTP-ответ
        const std::string_view response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Hello world!";

        net::write(socket, net::buffer(response));

        std::cout << "Response sent"sv << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: "sv << e.what() << std::endl;
        return 1;
    }
}
```

Сохранение:

* `Ctrl + O`
* Enter

Выход:

* `Ctrl + X`

---

# 2. Практическое задание: собрать программу

## Попытка сборки напрямую через g++

```bash
g++ -std=c++17 main.cpp -o server -lboost_system -lpthread
```

Ошибка:

```text
boost/asio/ip/tcp.hpp: No such file or directory
```

## Причина

Boost не установлен как системная библиотека.

---

## Решение: использовать CMake + Conan

### Создание conanfile.txt

```bash
nano conanfile.txt
```

```ini
[requires]
boost/1.83.0

[generators]
cmake

[options]
boost:header_only=False
```

---

### Создание CMakeLists.txt

```bash
nano CMakeLists.txt
```

```cmake
cmake_minimum_required(VERSION 3.15)
project(http_server_lesson9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(server main.cpp)

target_link_libraries(server ${CONAN_LIBS})
```

---

### Настройка профиля Conan

```bash
conan profile new default --detect --force
conan profile update settings.compiler.libcxx=libstdc++11 default
```

---

### Сборка проекта

```bash
mkdir -p build
cd build
conan install .. --build=missing
cmake ..
cmake --build .
```

Результат:

```text
[100%] Built target server
```

---

# 3. Практическое задание: запустить сервер

```bash
./bin/server
```

Вывод:

```text
HTTP server started on port 8080
Waiting for socket connection...
```

---

# 4. Практическое задание: проверить через curl

Во втором терминале:

```bash
curl http://127.0.0.1:8080/
```

---

## Результат

```text
Hello world!
```

---

## Что произошло

Клиент отправил запрос:

```http
GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: curl/8.5.0
Accept: */*
```

Сервер:

* принял соединение
* прочитал запрос
* отправил ответ

---

# 5. Практическое задание: проверить через браузер

## Запуск сервера

```bash
./bin/server
```

---

## Открытие в браузере

```text
http://127.0.0.1:8080/
```

---

## Результат

```text
Hello world!
```

---

## Что показал сервер

```text
Connection received
Request received:
GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: Mozilla/5.0 ...
Accept: ...
...
Response sent
```

---

# ⚠️ Ограничения сервера

* обрабатывает только одно соединение
* читает один запрос
* отправляет один ответ
* завершает работу

---

# 💡 Что это показывает

Полный цикл HTTP:

1. клиент открывает TCP-соединение
2. клиент отправляет HTTP-запрос
3. сервер читает запрос
4. сервер формирует HTTP-ответ
5. сервер отправляет ответ
6. клиент отображает результат

---

# 🔄 Альтернативная версия без CMake и Conan

Дополнительно была реализована версия:

```text
http_server_lesson9_1
```

---

## 📌 Идея

Сервер был собран напрямую через `g++`,
но с использованием уже скачанных зависимостей из кэша Conan.

---

## 📦 Где лежат зависимости

```text
~/.conan/data/
```

---

## 💾 Почему это важно

Установка:

```bash
sudo apt install libboost-all-dev
```

занимает около:

```text
~720 MB
```

---

## ✅ Что было сделано

* Boost не устанавливался через apt
* использовался уже скачанный Conan cache
* вручную указаны пути через `-I` и `-L`

---

## 🔧 Пример сборки

```bash
g++ -std=c++17 main.cpp -o server \
-I/home/ubuntu/.conan/data/boost/1.83.0/_/_/package/.../include \
-L/home/ubuntu/.conan/data/boost/1.83.0/_/_/package/.../lib \
-lboost_system -lpthread
```

---

## 📌 Вывод

Альтернативная версия показывает, что:

* можно обойтись без установки Boost
* можно переиспользовать Conan cache
* можно сэкономить ~720 MB дискового пространства
* можно собрать проект напрямую через g++

---

## 📎 Подробнее

См.:

```text
../http_server_lesson9_1/README.md
```

---

# 🏁 Итог

Выполнены все 5 практических заданий:

1. создан файл `main.cpp`
2. выполнена сборка через CMake + Conan
3. сервер запущен
4. выполнена проверка через curl
5. выполнена проверка через браузер

Дополнительно:

* реализована альтернативная версия через `g++`
* использован Conan cache
* сэкономлено ~720 MB дискового пространства

---

Сервер успешно:

* принимает HTTP-запросы
* отправляет корректный HTTP-ответ
* работает как с curl, так и с браузером
