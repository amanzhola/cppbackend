#include <boost/asio.hpp>
#include <cassert>
#include <iostream>
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
    net::io_context io{2};
    auto strand = net::make_strand(io);
    int value = 10;

    net::post(strand, [&value, &strand] {
        value += 20;  // (1)

        net::post(strand, [&value] {
            value += 10;  // (3)
        });
    });

    net::post(strand, [&value, &strand] {
        value *= 40;  // (2)

        net::post(strand, [&value] {
            value *= 20;  // (4)
        });
    });

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << "Final value = " << value << std::endl;

    assert(value == ((10 + 20) * 40 + 10) * 20);
}
