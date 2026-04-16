#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    for (const auto& header : req) {
        std::cout << "  " << header.name_string() << ": " << header.value() << std::endl;
    }
}

void HandleConnection(tcp::socket& socket) {
    beast::flat_buffer buffer;
    beast::error_code ec;

    StringRequest req;
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return;
    }
    if (ec) {
        std::cerr << "Read error: " << ec.message() << std::endl;
        return;
    }

    DumpRequest(req);

    StringResponse res{http::status::ok, req.version()};
    res.set(http::field::server, "Lesson10_10");
    res.set(http::field::content_type, "text/html; charset=utf-8");
    res.keep_alive(false);
    res.body() = "<strong>Hello</strong>\n";
    res.prepare_payload();

    http::write(socket, res, ec);
    if (ec) {
        std::cerr << "Write error: " << ec.message() << std::endl;
        return;
    }

    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    try {
        net::io_context ioc;

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        tcp::acceptor acceptor(ioc, {address, port});

        std::cout << "Server started on 0.0.0.0:8080" << std::endl;

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            HandleConnection(socket);
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
