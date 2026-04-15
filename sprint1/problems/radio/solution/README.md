# 📡 UDP Radio (WSL ↔ Mac) — версия 1.5 сек

---

## 📌 Описание

Проект реализует передачу аудио по UDP между разными системами:

* 🎤 запись звука на Mac (client)
* 📦 разбиение на чанки
* 🌐 отправка по UDP
* 📥 приём и сборка в WSL (server)
* 🔊 воспроизведение звука

---

## 🎯 Цель

Реализовать и понять:

* передачу бинарных данных по UDP
* собственный сетевой протокол
* работу аудио через `miniaudio`
* взаимодействие Mac ↔ WSL (Windows)

---

# 🏗️ Структура проекта

## 🖥️ Сервер (WSL)

```text
solution/
├── CMakeLists.txt
├── conanfile.txt
├── README.md
├── src/
│   ├── audio.h       # Player (miniaudio)
│   └── main.cpp      # UDP сервер
└── build/
    ├── bin/
    │   └── radio     # исполняемый файл
    └── ...           # CMake / Conan файлы
```

---

## 🍎 Клиент (Mac)

```text
radio_mac/
├── audio.h       # только Recorder
├── main.cpp      # client-only (1.5 сек запись)
├── miniaudio.h   # библиотека
└── client        # бинарник
```

---

# ⚙️ Архитектура

```text
Mac (client)
  🎤 запись (Recorder)
      ↓
  📦 chunking + header
      ↓
  🌐 UDP
      ↓
WSL (server)
  📥 приём чанков
      ↓
  🧩 сборка сообщения
      ↓
  🔊 воспроизведение (Player)
```

---

# 🧨 Главная проблема (почему НЕ БЫЛО ЗВУКА)

Ситуация выглядела идеально:

```text
✔ UDP пакеты приходят
✔ чанки собираются
✔ Message complete есть
❌ звука НЕТ
```

---

## 💥 Реальная ошибка

Вот эта строка:

```cpp
ma_device_init(nullptr, &device_config, &device_);
```

---

## ❗ Почему это ломает WSL

Это означает:

```text
miniaudio сам выбирает backend
```

Но в WSL:

* ALSA ❌ (часто не работает)
* PulseAudio ✅ (через WSLg)
* виртуальные устройства ❌

👉 miniaudio выбирает НЕ тот backend

---

## 💣 Симптом

```text
Playing done
```

НО:

```text
🔇 тишина
```

---

# 🔥 Почему раньше работало

Использовались:

```bash
paplay
parecord
```

И:

```bash
export PULSE_SERVER=unix:/mnt/wslg/PulseServer
```

👉 они гарантированно используют PulseAudio

---

# 🔧 Исправление

Был принудительно задан backend:

```cpp
const ma_backend backends[] = {ma_backend_pulseaudio};
ma_context_init(backends, 1, ...);
ma_device_init(&context_, ...);
```

---

## ✅ Результат

```text
звук появился
```

---

# 🧠 Ключевой вывод

```text
WSL ≠ обычный Linux
```

---

## Для аудио:

```text
ВСЕГДА использовать PulseAudio backend
```

---

# 🚀 Что теперь работает

```text
Mac 🎤
  ↓
UDP
  ↓
WSL 📥
  ↓
PulseAudio
  ↓
WSLg
  ↓
Windows 🔊
```

---

# 📊 Пример логов

## 📤 Клиент (Mac)

```text
Recording 1.5 seconds...
Recording done
Total bytes: ...
Sent chunk 1/...
...
All chunks sent successfully
```

---

## 📥 Сервер (WSL)

```text
RAW packet: ...
Accepted chunk ...
...
Message complete
Playing done
```

---

# ⚡ Особенности реализации

## 📦 Протокол

```cpp
struct PacketHeader {
    uint32_t magic;
    uint32_t message_id;
    uint32_t total_size;
    uint16_t chunk_index;
    uint16_t chunk_count;
    uint16_t payload_size;
};
```

---

## 🧩 Реализовано

* ✔ chunking (~1400 bytes)
* ✔ message_id
* ✔ сборка вне порядка
* ✔ защита от дублей
* ✔ TTL очистка

---

## ⏱ Почему 1.5 секунды

* быстрее тестировать
* меньше чанков
* меньше потерь UDP
* быстрее цикл разработки

---

# 🚀 Запуск

## ▶️ Сервер (WSL)

```bash
cd build
export PULSE_SERVER=unix:/mnt/wslg/PulseServer
./bin/radio server 3333
```

---

## ▶️ Клиент (Mac)

```bash
clang++ -std=c++20 main.cpp -o client
./client
```

---

## 📥 Ввод IP

```text
192.168.0.xxx
```

---

# 🎉 Итог

Удалось добиться:

* стабильной передачи аудио
* работы Mac → WSL
* корректного воспроизведения
* устранения “тихого бага”

---

# 🧠 Главный вывод

```text
если всё работает, но нет результата
→ проблема часто в среде, а не в логике
```

---

# 💡 Чему научились

* UDP требует своего протокола
* аудио backend критичен
* WSL имеет особенности
* лог ≠ реальный результат

---

# 🏁 Финал

```text
Mac 🎤 → UDP → WSL 🔊
```

Это уже полноценная **межплатформенная система передачи аудио** 🚀
