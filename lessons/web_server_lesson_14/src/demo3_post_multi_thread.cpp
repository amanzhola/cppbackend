#include <boost/asio.hpp>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>
#include <chrono>

namespace net = boost::asio;

template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    std::vector<std::jthread> workers;
    workers.reserve(n);

    while (n--) {
        workers.emplace_back(fn);
    }
}

int main() {
    using osync = std::osyncstream;

    net::io_context io;
    std::cout << "Eat. Thread id: " << std::this_thread::get_id() << std::endl;

    net::post(io, [] {
        osync(std::cout) << "Wash dishes. Thread id: "
                         << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    net::post(io, [] {
        osync(std::cout) << "Cleanup table. Thread id: "
                         << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    net::post(io, [&io] {
        osync(std::cout) << "Vacuum-clean room. Thread id: "
                         << std::this_thread::get_id() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        net::post(io, [] {
            osync(std::cout) << "Wash floor. Thread id: "
                             << std::this_thread::get_id() << std::endl;
        });

        net::post(io, [] {
            osync(std::cout) << "Empty vacuum cleaner. Thread id: "
                             << std::this_thread::get_id() << std::endl;
        });
    });

    std::cout << "Work. Thread id: " << std::this_thread::get_id() << std::endl;

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << "Sleep" << std::endl;
}
