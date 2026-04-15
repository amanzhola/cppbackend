#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string_view>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

int main() {
    net::io_context ioc;

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, {address, port});

    std::cout << "Waiting for socket connection"sv << std::endl;

    tcp::socket socket(ioc);
    acceptor.accept(socket);

    std::cout << "Connection received"sv << std::endl;
}
