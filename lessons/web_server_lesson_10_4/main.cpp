#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

void HandleRequest(const http::request<http::string_body>& req, tcp::socket& socket) {
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, "Lesson10_4");
    res.set(http::field::content_type, "text/plain; charset=utf-8");
    res.keep_alive(req.keep_alive());

    if (req.target() == "/hello") {
        res.body() = "Hello!\n";
    } else if (req.target() == "/bye") {
        res.body() = "Goodbye!\n";
    } else {
        res.result(http::status::not_found);
        res.body() = "404\n";
    }

    res.prepare_payload();
    http::write(socket, res);
}

void HandleConnection(tcp::socket& socket) {
    beast::flat_buffer buffer;

    while (true) {
        http::request<http::string_body> req;
        beast::error_code ec;

        http::read(socket, buffer, req, ec);

        if (ec == http::error::end_of_stream) {
            std::cout << "Client disconnected\n";
            break;
        }

        if (ec) {
            std::cerr << "Read error: " << ec.message() << '\n';
            break;
        }

        std::cout << "Request received: " << req.method_string()
                  << " " << req.target() << '\n';

        HandleRequest(req, socket);

        if (!req.keep_alive()) {
            std::cout << "Connection will be closed by client\n";
            break;
        }
    }

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    try {
        net::io_context ioc;

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "Server started on 0.0.0.0:8080\n";

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            std::cout << "Client connected\n";
            HandleConnection(socket);
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}
