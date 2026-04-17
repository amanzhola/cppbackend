#include <boost/asio.hpp>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

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
    auto strand = net::make_strand(io);

    net::post(strand, [strand] {
        net::post(strand, [] {
            osync(std::cout) << 'A';
        });
        net::dispatch(strand, [] {
            osync(std::cout) << 'B';
        });
        osync(std::cout) << 'C';
    });

    net::post(io, [&io] {
        net::post(io, [] {
            osync(std::cout) << 'D';
        });
        net::dispatch(io, [] {
            osync(std::cout) << 'E';
        });
        osync(std::cout) << 'F';
    });

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << std::endl;
    std::cout << "Relative order for A, B, C: BCA" << std::endl;
}
