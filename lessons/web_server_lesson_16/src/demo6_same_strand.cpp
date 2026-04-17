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

    net::post(strand, [strand] {  // (1)
        net::post(strand, [] {    // (2)
            osync(std::cout) << 'A';
        });
        net::dispatch(strand, [] {  // (3)
            osync(std::cout) << 'B';
        });
        osync(std::cout) << 'C';
    });

    // Теперь тут используется strand, а не io
    net::post(strand, [strand] {  // (4)
        net::post(strand, [] {    // (5)
            osync(std::cout) << 'D';
        });
        net::dispatch(strand, [] {  // (6)
            osync(std::cout) << 'E';
        });
        osync(std::cout) << 'F';
    });

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << std::endl;
    std::cout << "Output order becomes strictly determined." << std::endl;
}
