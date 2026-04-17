#include <boost/asio.hpp>
#include <iostream>

namespace net = boost::asio;

int main() {
    net::io_context io;

    net::post(io, [&io] {
        std::cout << "Read book" << std::endl;

        net::post(io, [] {
            std::cout << "Write essay" << std::endl;
        });
    });

    net::post(io, [] {
        std::cout << "Play football" << std::endl;
    });

    std::cout << "Watch movie" << std::endl;
    io.run();
    std::cout << "Study English" << std::endl;
}
