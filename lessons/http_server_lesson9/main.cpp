#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <array>
#include <string_view>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

int main() {
    try {
        // Контекст для операций ввода-вывода
        net::io_context ioc;

        // Слушаем все IPv4-интерфейсы на порту 8080
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "HTTP server started on port 8080"sv << std::endl;
        std::cout << "Waiting for socket connection..."sv << std::endl;

        // Сокет для общения с клиентом
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        std::cout << "Connection received"sv << std::endl;

        // Буфер для чтения запроса клиента
        std::array<char, 4096> buffer{};
        const std::size_t bytes_read = socket.read_some(net::buffer(buffer));

        std::cout << "Request received:"sv << std::endl;
        std::cout.write(buffer.data(), static_cast<std::streamsize>(bytes_read));
        std::cout << std::endl;

        // Фиксированный HTTP-ответ
        const std::string_view response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Hello world!";

        net::write(socket, net::buffer(response));

        std::cout << "Response sent"sv << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: "sv << e.what() << std::endl;
        return 1;
    }
}
