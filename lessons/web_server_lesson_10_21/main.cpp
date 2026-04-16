#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

using namespace std::literals;

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;

    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
};

std::string GetCurrentTimeString() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &tm);
#endif

    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

StringResponse MakeStringResponse(
    http::status status,
    std::string body,
    unsigned version,
    bool keep_alive,
    std::string_view content_type = ContentType::TEXT_HTML)
{
    StringResponse response(status, version);
    response.set(http::field::content_type, content_type);
    response.body() = std::move(body);
    response.content_length(response.body().size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    const auto make_response =
        [&req](http::status status, std::string body, std::string_view content_type = ContentType::TEXT_HTML) {
            return MakeStringResponse(status, std::move(body), req.version(), req.keep_alive(), content_type);
        };

    if (req.method() == http::verb::get && req.target() == "/") {
        return make_response(http::status::ok, "<h1>Main page</h1>\n"s);
    }

    if (req.method() == http::verb::get && req.target() == "/hello") {
        return make_response(http::status::ok, "<strong>Hello</strong>\n"s);
    }

    if (req.method() == http::verb::get && req.target() == "/time") {
        return make_response(
            http::status::ok,
            "<p>Server time: "s + GetCurrentTimeString() + "</p>\n"s);
    }

    if (req.method() == http::verb::get && req.target() == "/json") {
        return make_response(
            http::status::ok,
            "{\"message\":\"Hello\",\"status\":\"ok\"}\n"s,
            ContentType::APPLICATION_JSON);
    }

    if (req.target() == "/echo") {
        if (req.method() != http::verb::post) {
            return make_response(
                http::status::method_not_allowed,
                "Method Not Allowed\n"s,
                ContentType::TEXT_PLAIN);
        }

        return make_response(
            http::status::ok,
            req.body() + "\n"s,
            ContentType::TEXT_PLAIN);
    }

    return make_response(
        http::status::not_found,
        "404 Not Found\n"s,
        ContentType::TEXT_PLAIN);
}

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

template <typename RequestHandler>
void HandleConnection(tcp::socket socket, RequestHandler&& handler) {
    try {
        beast::flat_buffer buffer;

        while (auto request = ReadRequest(socket, buffer)) {
            DumpRequest(*request);

            auto response = handler(std::move(*request));
            http::write(socket, response);

            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor(ioc, {net::ip::make_address("0.0.0.0"), 8080});

        std::cout << "Server started on port 8080" << std::endl;

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
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
