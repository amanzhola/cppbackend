#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

// ---------- ROUTER ----------
http::response<http::string_body>
MakeResponse(const http::request<http::string_body>& req) {
    http::response<http::string_body> res;
    res.version(req.version());
    res.set(http::field::server, "Lesson10_6");
    res.set(http::field::content_type, "text/plain; charset=utf-8");
    res.keep_alive(req.keep_alive());

    // 🔥 СНАЧАЛА ПРОВЕРКА МЕТОДА
    if (req.method() != http::verb::get) {
        res.result(http::status::method_not_allowed);
        res.body() = "Only GET method is supported\n";
        res.content_length(res.body().size());
        return res;
    }

    // 🔥 ПОТОМ МАРШРУТЫ
    if (req.target() == "/") {
        res.result(http::status::ok);
        res.body() = "Main page\n";
    } else if (req.target() == "/hello") {
        res.result(http::status::ok);
        res.body() = "Hello!\n";
    } else if (req.target() == "/time") {
        res.result(http::status::ok);
        res.body() = "This could be current server time\n";
    } else {
        res.result(http::status::not_found);
        res.body() = "404 Not Found\n";
    }

    res.content_length(res.body().size());
    return res;
}

// ---------- CONNECTION ----------
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

        std::cout << "Request: " << req.method_string()
                  << " " << req.target() << '\n';

        auto res = MakeResponse(req);
        http::write(socket, res);

        if (!req.keep_alive()) {
            break;
        }
    }

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

// ---------- MAIN ----------
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
    }
}
