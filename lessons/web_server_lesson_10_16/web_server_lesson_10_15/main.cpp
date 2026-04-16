#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

#include <iostream>

namespace net = boost::asio;
namespace http = boost::beast::http;
namespace beast = boost::beast;
using tcp = net::ip::tcp;

int main() {
    net::io_context ioc;

    tcp::acceptor acceptor(ioc, {net::ip::make_address("0.0.0.0"), 8080});

    std::cout << "Server started\n";

    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        beast::error_code ec;
        http::read(socket, buffer, req, ec);

        if (ec) continue;

        std::cout << req.method_string() << " " << req.target() << std::endl;

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::content_type, "text/html; charset=utf-8");

        // тело
        res.body() = "<strong>Hello</strong>";

        // ВАЖНО: ломаем длину
        res.content_length(res.body().size() / 2);

        // ВАЖНО: закрываем соединение
        res.keep_alive(false);

        http::write(socket, res, ec);

        socket.shutdown(tcp::socket::shutdown_send, ec);
    }
}
