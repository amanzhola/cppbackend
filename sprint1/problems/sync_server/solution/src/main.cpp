#ifdef WIN32
#include <sdkddkver.h>
#endif
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdlib>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
};

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) return std::nullopt;
    if (ec) throw std::runtime_error(ec.message());

    return req;
}

StringResponse MakeStringResponse(http::status status,
                                  unsigned version,
                                  bool keep_alive,
                                  std::string_view body) {
    StringResponse response(status, version);
    response.set(http::field::content_type, ContentType::TEXT_HTML);
    response.body() = std::string(body);
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse MakeMethodNotAllowedResponse(unsigned version, bool keep_alive) {
    constexpr std::string_view body = "Invalid method"sv;
    auto res = MakeStringResponse(http::status::method_not_allowed, version, keep_alive, body);
    res.set(http::field::allow, "GET, HEAD"sv);
    return res;
}

std::string GetTarget(std::string_view target) {
    if (!target.empty() && target.front() == '/') {
        target.remove_prefix(1);
    }
    return std::string(target);
}

StringResponse HandleRequest(StringRequest&& req) {
    if (req.method() != http::verb::get && req.method() != http::verb::head) {
        return MakeMethodNotAllowedResponse(req.version(), req.keep_alive());
    }

    std::string name = GetTarget(req.target());
    std::string body = "Hello, " + name;

    StringResponse res(http::status::ok, req.version());
    res.set(http::field::content_type, ContentType::TEXT_HTML);
    res.content_length(body.size());
    res.keep_alive(req.keep_alive());

    if (req.method() == http::verb::get) {
        res.body() = body;
    }

    return res;
}

template <typename Handler>
void HandleConnection(tcp::socket socket, Handler handler) {
    try {
        beast::flat_buffer buffer;

        while (auto req = ReadRequest(socket, buffer)) {
            auto res = handler(std::move(*req));
            http::write(socket, res);

            if (res.need_eof()) break;
        }
    } catch (...) {}

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    net::io_context ioc;

    auto address = net::ip::make_address("0.0.0.0");
    unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, {address, port});

    std::cout << "Server has started..." << std::endl;

    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        std::thread(
            [](tcp::socket socket) {
                HandleConnection(std::move(socket), HandleRequest);
            },
            std::move(socket)
        ).detach();
    }
}
