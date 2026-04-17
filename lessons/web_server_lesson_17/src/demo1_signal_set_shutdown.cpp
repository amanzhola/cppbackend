#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <string>

#include "common.h"

using namespace std::literals;

// Demo 1:
// Корректное завершение работы сервера с помощью boost::asio::signal_set.
// Идея:
// 1) создаём io_context;
// 2) подписываемся на SIGINT и SIGTERM;
// 3) запускаем асинхронный таймер;
// 4) если приходит сигнал, вызываем io_context::stop();
// 5) после остановки всех worker-потоков печатаем "Shutting down".
int main() {
    const unsigned num_threads =
        std::max(1u, std::thread::hardware_concurrency());

    net::io_context ioc(static_cast<int>(num_threads));

    std::cout << "Demo 1: signal_set shutdown" << std::endl;
    std::cout << "Workers: " << num_threads << std::endl;
    std::cout << "Press Ctrl+C or send SIGTERM to stop the server gracefully."
              << std::endl;

    // Подписываемся на SIGINT и SIGTERM.
    // SIGINT обычно приходит при Ctrl+C.
    // SIGTERM обычно отправляют через kill.
    net::signal_set signals(ioc, SIGINT, SIGTERM);

    signals.async_wait([&ioc](const sys::error_code& ec, int signal_number) {
        if (!ec) {
            std::cout << "Signal " << signal_number << " received" << std::endl;
            ioc.stop();
        } else {
            std::cout << "Signal wait error: " << ec.message() << std::endl;
        }
    });

    // Таймер на 30 секунд.
    // Если не отправить сигнал раньше, он просто истечёт сам.
    net::steady_timer timer{ioc, std::chrono::seconds(30)};
    timer.async_wait([](const sys::error_code& ec) {
        if (!ec) {
            std::cout << "Timer expired" << std::endl;
        } else {
            std::cout << "Timer cancelled: " << ec.message() << std::endl;
        }
    });

    RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });

    std::cout << "Shutting down" << std::endl;
}
