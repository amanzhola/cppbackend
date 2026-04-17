#include <boost/asio.hpp>
#include <iostream>

namespace net = boost::asio;

int main() {
    net::io_context io;

    // dispatch поместит вызов лямбда-функции в очередь,
    // так как вызван вне io.run()
    net::dispatch(io, [&io] {
        std::cout << "Work hard" << std::endl;

        // post всегда выполняет функцию асинхронно
        net::post(io, [] {
            std::cout << "Have dinner" << std::endl;
        });

        // Этот dispatch вызовет лямбду синхронно,
        // так как dispatch вызван внутри io.run()
        net::dispatch(io, [] {
            std::cout << "Eat an apple" << std::endl;
        });

        std::cout << "Go home" << std::endl;
    });

    std::cout << "Go to work" << std::endl;
    io.run();
    std::cout << "Sleep" << std::endl;
}
