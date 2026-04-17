#include <boost/asio.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::chrono;
using namespace std::literals;

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);

    std::vector<std::jthread> workers;
    workers.reserve(n - 1);

    while (--n) {
        workers.emplace_back(fn);
    }

    fn();
}

int main() {
    const unsigned num_threads = 2;
    net::io_context io{num_threads};

    net::steady_timer t1{io, 400ms};
    net::steady_timer t2{io, 600ms};
    net::steady_timer t3{io, 800ms};
    net::steady_timer t4{io, 1000ms};

    auto make_timer_handler = [](int index) {
        return [index](sys::error_code) {
            std::osyncstream{std::cout} << "Enter #" << index << std::endl;
            std::this_thread::sleep_for(1s);
            std::osyncstream{std::cout} << "Exit  #" << index << std::endl;
        };
    };

    auto strand1 = net::make_strand(io);
    auto strand2 = net::make_strand(io);

    t1.async_wait(net::bind_executor(strand1, make_timer_handler(1)));
    t2.async_wait(net::bind_executor(strand1, make_timer_handler(2)));
    t3.async_wait(net::bind_executor(strand2, make_timer_handler(3)));
    t4.async_wait(net::bind_executor(strand2, make_timer_handler(4)));

    RunWorkers(num_threads, [&io] {
        io.run();
    });
}
