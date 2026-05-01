#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/json.hpp>

#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

#include "json_loader.h"
#include "logger.h"
#include "request_handler.h"

using namespace std::literals;

namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;
namespace json = boost::json;

namespace {

template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);

    std::vector<std::jthread> workers;
    workers.reserve(n - 1);

    while (--n) {
        workers.emplace_back(fn);
    }

    fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <static-root>"sv << std::endl;
        return EXIT_FAILURE;
    }

    try {
        model::Game game = json_loader::LoadGame(argv[1]);
        fs::path static_root = argv[2];

        InitLogging();

        const unsigned num_threads = std::max(1u, std::thread::hardware_concurrency());
        net::io_context ioc(static_cast<int>(num_threads));

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                ioc.stop();
            }
        });

        http_handler::RequestHandler handler{game, static_root};

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;

        LogInfo("server started", json::object{
            {"port", port},
            {"address", address.to_string()}
        });

        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            handler(
                std::forward<decltype(req)>(req),
                std::forward<decltype(send)>(send)
            );
        });

        RunWorkers(num_threads, [&ioc] {
            ioc.run();
        });

        LogInfo("server exited", json::object{
            {"code", 0}
        });

        return EXIT_SUCCESS;

    } catch (const std::exception& ex) {
        InitLogging();

        LogInfo("server exited", json::object{
            {"code", EXIT_FAILURE},
            {"exception", ex.what()}
        });

        return EXIT_FAILURE;
    }
}
