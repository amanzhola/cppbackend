#include <boost/asio.hpp>
#include <iostream>
#include <thread>

namespace net = boost::asio;

int main() {
    net::io_context io;

    std::cout << "Eat. Thread id: " << std::this_thread::get_id() << std::endl;

    net::post(io, [] {
        std::cout << "Wash dishes. Thread id: "
                  << std::this_thread::get_id() << std::endl;
    });

    net::post(io, [] {
        std::cout << "Cleanup table. Thread id: "
                  << std::this_thread::get_id() << std::endl;
    });

    std::cout << "Work" << std::endl;
    io.run();
    std::cout << "Sleep" << std::endl;
}
