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
    res.keep_alive(false);

    if (req.method() == http::verb::post && req.target() == "/echo") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json; charset=utf-8");
        res.body() = std::string("{\"echo\":\"") + req.body() + "\"}\n";
    } else if (req.method() == http::verb::get && req.target() == "/") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json; charset=utf-8");
        res.body() = "{\"message\":\"Main page\"}\n";
    } else if (req.method() == http::verb::get && req.target() == "/hello") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json; charset=utf-8");
        res.body() = "{\"message\":\"Hello!\"}\n";
    } else if (req.method() == http::verb::get && req.target() == "/bye") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json; charset=utf-8");
        res.body() = "{\"message\":\"Goodbye!\"}\n";
    } else if (req.method() == http::verb::get && req.target() == "/html") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "text/html; charset=utf-8");
        res.body() =
            "<!doctype html>"
            "<html>"
            "<head><title>Test</title></head>"
            "<body><h1>Hello from HTML page</h1></body>"
            "</html>";
    } else {
        res.result(http::status::not_found);
        res.set(http::field::content_type, "application/json; charset=utf-8");
        res.body() = "{\"error\":\"Not found\"}\n";
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
            if (!req.body().empty()) {
                std::cout << "Body: " << req.body() << std::endl;
            }

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
