# HTTP Server (без Conan/CMake, через g++)

## 📌 Задача

Собрать простой HTTP-сервер на Boost.Asio без использования CMake и Conan, напрямую через `g++`.

---

## ❗ Проблема

При попытке собрать:

```
g++ -std=c++17 main.cpp -o server -lboost_system -lpthread
```

возникает ошибка:

```
boost/asio/ip/tcp.hpp: No such file or directory
```

Это происходит потому, что Boost не установлен в системе.

---

## 🚫 Почему не использовал apt

Установка:

```
sudo apt install libboost-all-dev
```

требует около **720 MB дискового пространства**.

---

## ✅ Решение

Вместо установки Boost через `apt`, был использован уже скачанный Boost из Conan.

Conan хранит зависимости в:

```
~/.conan/data/
```

---

## 🔧 Шаги

### 1. Найти заголовки Boost

```
find ~/.conan -type d -path "*/package/*/include/boost"
```

### 2. Найти библиотеку boost_system

```
find ~/.conan -type f -name "libboost_system.a"
```

### 3. Скомпилировать программу

```
g++ -std=c++17 main.cpp -o server \
-I/home/ubuntu/.conan/data/boost/1.83.0/_/_/package/c292dd476ad84dadc62ba2afdc71ee922e5eaf27/include \
-L/home/ubuntu/.conan/data/boost/1.83.0/_/_/package/c292dd476ad84dadc62ba2afdc71ee922e5eaf27/lib \
-lboost_system -lpthread
```

---

## 🚀 Запуск

```
./server
```

В другом терминале:

```
curl http://127.0.0.1:8080/
```

---

## 📥 Результат

```
Hello world!
```

---

## 💾 Преимущества подхода

* не требуется установка Boost (~720 MB)
* используется уже скачанная версия из Conan
* экономия дискового пространства
* отсутствие дублирования библиотек

---

## ⚠️ Замечание

Такой способ:

* подходит для учебных задач
* требует ручного указания путей (`-I`, `-L`)

В реальных проектах предпочтительно использовать:

* CMake + Conan
