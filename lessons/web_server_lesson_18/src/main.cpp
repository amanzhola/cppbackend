#include <boost/asio/signal_set.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <string>
#include <string_view>
#include <thread>

#include "common.h"
#include "http_server.h"

namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

StringResponse HandleRequest(StringRequest&& req) {
    auto make_text_response = [&req](http::status status, std::string_view text) {
        StringResponse response{status, req.version()};
        response.set(http::field::content_type, "text/plain");
        response.body() = std::string(text);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };

    if (req.method() != http::verb::get) {
        return make_text_response(http::status::method_not_allowed,
                                  "Only GET requests are supported");
    }

    if (req.target() == "/") {
        return make_text_response(http::status::ok,
                                  "Hello from async HTTP server lesson 18");
    }

    if (req.target() == "/ping") {
        return make_text_response(http::status::ok, "pong");
    }

    if (req.target() == "/hello") {
        return make_text_response(http::status::ok, "hello");
    }

    return make_text_response(http::status::not_found, "Resource not found");
}

int main() {
    try {
        const auto num_threads = std::max(1u, std::thread::hardware_concurrency());

        net::io_context ioc(static_cast<int>(num_threads));

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Signal " << signal_number << " received" << std::endl;
                ioc.stop();
            }
        });

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;

        http_server::ServeHttp(ioc, {address, port}, [](auto&& req, auto&& sender) {
            sender(HandleRequest(std::forward<decltype(req)>(req)));
        });

        std::cout << "Lesson 18 async HTTP server is running on http://0.0.0.0:8080"
                  << std::endl;
        std::cout << "Try: curl http://127.0.0.1:8080/" << std::endl;
        std::cout << "Try: curl http://127.0.0.1:8080/ping" << std::endl;
        std::cout << "Stop with Ctrl+C or kill <PID>" << std::endl;

        RunWorkers(num_threads, [&ioc] {
            ioc.run();
        });

        std::cout << "Server stopped" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
