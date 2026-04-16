#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <optional>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;

    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        std::cerr << "Read error: " << ec.message() << std::endl;
        return std::nullopt;
    }

    return req;
}

void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    for (const auto& header : req) {
        std::cout << "  " << header.name_string() << ": " << header.value() << std::endl;
    }
}

StringResponse MakeResponse(const StringRequest& req) {
    if (req.method() != http::verb::get) {
        StringResponse response{http::status::method_not_allowed, req.version()};
        response.set(http::field::server, "Lesson10_14");
        response.set(http::field::content_type, "text/plain; charset=utf-8");
        response.keep_alive(req.keep_alive());
        response.body() = "Only GET method is supported\n";
        response.prepare_payload();
        return response;
    }

    StringResponse response{http::status::ok, req.version()};
    response.set(http::field::server, "Lesson10_14");
    response.keep_alive(req.keep_alive());

    if (req.target() == "/") {
        response.set(http::field::content_type, "text/html; charset=utf-8");
        response.body() = "<h1>Main page</h1>\n";
    } else if (req.target() == "/hello") {
        response.set(http::field::content_type, "text/html; charset=utf-8");
        response.body() = "<strong>Hello</strong>\n";
    } else {
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/plain; charset=utf-8");
        response.body() = "404 Not Found\n";
    }

    response.prepare_payload();
    return response;
}

void HandleConnection(tcp::socket& socket) {
    beast::flat_buffer buffer;

    while (true) {
        auto request = ReadRequest(socket, buffer);
        if (!request) {
            break;
        }

        DumpRequest(*request);

        auto response = MakeResponse(*request);

        beast::error_code ec;
        http::write(socket, response, ec);
        if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
            break;
        }

        if (!response.keep_alive()) {
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

        std::cout << "Server started on 0.0.0.0:8080" << std::endl;

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            std::cout << "New client connected" << std::endl;
            HandleConnection(socket);
            std::cout << "Client session finished" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
