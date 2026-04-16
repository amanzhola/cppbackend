#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace std::literals;

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
        throw std::runtime_error("Failed to read request: "s + ec.message());
    }
    return req;
}

void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
}

void HandleConnection(tcp::socket& socket) {
    try {
        beast::flat_buffer buffer;

        while (auto request = ReadRequest(socket, buffer)) {
            DumpRequest(*request);

            StringResponse response(http::status::ok, request->version());
            response.set(http::field::server, "Lesson10_9");
            response.set(http::field::content_type, "text/html; charset=utf-8"sv);
            response.body() = "<strong>Hello</strong>\n"s;
            response.content_length(response.body().size());
            response.keep_alive(request->keep_alive());

            http::write(socket, response);

            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
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
}
