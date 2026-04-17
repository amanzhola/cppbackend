#include <boost/asio.hpp>
#include <iostream>

namespace net = boost::asio;

int main() {
    net::io_context io1;
    net::io_context io2;
    auto strand = net::make_strand(io1);

    net::dispatch(io1, [&io2, &strand] {
        // Эта функция будет вызвана в контексте io1
        std::cout << "Enter io1 lambda" << std::endl;

        net::dispatch(io2, [] {
            // Эта функция будет вызвана асинхронно в контексте io2
            std::cout << "Execute in io2" << std::endl;
        });

        net::dispatch(strand, [] {
            // Эта функция будет вызвана синхронно внутри strand в контексте io1
            std::cout << "Execute in strand" << std::endl;
        });

        std::cout << "Leave io1 lambda" << std::endl;
    });

    std::cout << "io1.run()" << std::endl;
    io1.run();
    std::cout << "io2.run()" << std::endl;
    io2.run();
}
