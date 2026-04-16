#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = net::ip::tcp;

http::response<http::string_body> MakeStringResponse(
    http::status status,
    std::string body,
    unsigned version,
    bool keep_alive,
    std::string content_type = "text/plain; charset=utf-8") 
{
    http::response<http::string_body> res{status, version};
    res.set(http::field::content_type, content_type);
    res.body() = std::move(body);
    res.content_length(res.body().size());
    res.keep_alive(keep_alive);
    return res;
}

http::response<http::string_body> HandleRequest(const http::request<http::string_body>& req) {
    if (req.method() != http::verb::get) {
        return MakeStringResponse(
            http::status::method_not_allowed,
            "Only GET method is supported\n",
            req.version(),
            false
        );
    }

    if (req.target() == "/hello") {
        return MakeStringResponse(
            http::status::ok,
            "Hello from server!\n",
            req.version(),
            false
        );
    }

    return MakeStringResponse(
        http::status::not_found,
        "Resource not found\n",
        req.version(),
        false
    );
}

void HandleConnection(tcp::socket& socket) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;

    http::read(socket, buffer, req);

    std::cout << "[INFO] Request: "
              << req.method_string() << " " << req.target() << std::endl;

    auto res = HandleRequest(req);
    http::write(socket, res);

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    try {
        net::io_context ioc;

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "[INFO] Server started on http://0.0.0.0:" << port << std::endl;
        std::cout << "[INFO] Waiting for connections..." << std::endl;

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            std::cout << "[INFO] Client connected" << std::endl;
            HandleConnection(socket);
            std::cout << "[INFO] Client processed, server continues working\n" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
