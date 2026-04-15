#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::chrono;
using namespace std::literals;
using namespace std::string_view_literals;

int main() {
    net::io_context io;
    const auto start_time = steady_clock::now();

    // Жарим яичницу 3 секунды
    {
        net::steady_timer t(io, 3s);
        std::cout << "Fry eggs"sv << std::endl;
        t.async_wait([](sys::error_code ec) {
            if (ec) {
                throw std::runtime_error("Wait error: "s + ec.message());
            }
            std::cout << "Put eggs onto the plate"sv << std::endl;
        });
    }

    // Варим кофе 5 секунд
    {
        net::steady_timer t(io, 5s);
        std::cout << "Brew coffee"sv << std::endl;
        t.async_wait([](sys::error_code ec) {
            if (ec) {
                throw std::runtime_error("Wait error: "s + ec.message());
            }
            std::cout << "Pour coffee in the cup"sv << std::endl;
        });
    }

    std::cout << "Run asynchronous operations"sv << std::endl;
    io.run();

    const auto cook_duration = duration<double>(steady_clock::now() - start_time);
    std::cout << "Breakfast has been cooked in "
              << cook_duration.count()
              << "s"sv << std::endl;
    std::cout << "Enjoy your meal"sv << std::endl;
}
