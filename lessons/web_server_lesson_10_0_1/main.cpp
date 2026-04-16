#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

http::response<http::string_body>
make_response(const http::request<http::string_body>& req) {
    http::response<http::string_body> res;
    res.version(req.version());
    res.set(http::field::server, "Lesson Server");
    res.set(http::field::content_type, "application/json; charset=utf-8");
    res.keep_alive(false);

    if (req.method() != http::verb::get) {
        res.result(http::status::method_not_allowed);
        res.body() = "{\"error\":\"Only GET is supported\"}\n";
    } else if (req.target() == "/") {
        res.result(http::status::ok);
        res.body() = "{\"message\":\"Main page\"}\n";
    } else if (req.target() == "/hello") {
        res.result(http::status::ok);
        res.body() = "{\"message\":\"Hello!\"}\n";
    } else if (req.target() == "/bye") {
        res.result(http::status::ok);
        res.body() = "{\"message\":\"Goodbye!\"}\n";
    } else {
        res.result(http::status::not_found);
        res.body() = "{\"error\":\"404\"}\n";
    }

    res.content_length(res.body().size());
    return res;
}

int main() {
    try {
        net::io_context ioc;

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "Server started on port 8080" << std::endl;
        std::cout << "Waiting for connection..." << std::endl;

        for (;;) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            std::cout << "Connection received" << std::endl;

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::cout << req.method_string() << " " << req.target() << std::endl;

            auto res = make_response(req);
            http::write(socket, res);

            beast::error_code ec;
            socket.shutdown(tcp::socket::shutdown_send, ec);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
