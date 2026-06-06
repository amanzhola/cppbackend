#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/json.hpp>
#include <boost/program_options.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "json_loader.h"
#include "logger.h"
#include "players.h"
#include "player_tokens.h"
#include "request_handler.h"
#include "state_serialization.h"
#include "ticker.h"

using namespace std::literals;

namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;
namespace json = boost::json;

namespace {

struct Args {
    fs::path config_file;
    fs::path www_root;
    std::optional<int> tick_period;
    bool randomize_spawn_points = false;
    std::optional<fs::path> state_file;
    std::optional<int> save_state_period;
};

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
    namespace po = boost::program_options;

    Args args;

    po::options_description desc{"Allowed options"s};

    desc.add_options()
        ("help,h", "produce help message")
        ("tick-period,t", po::value<int>()->value_name("milliseconds"), "set tick period")
        ("config-file,c", po::value<std::string>()->value_name("file")->required(), "set config file path")
        ("www-root,w", po::value<std::string>()->value_name("dir")->required(), "set static files root")
        ("randomize-spawn-points", "spawn dogs at random positions")
        ("state-file", po::value<std::string>()->value_name("file"), "set state file path")
        ("save-state-period", po::value<int>()->value_name("milliseconds"), "set state save period");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.contains("help"s)) {
        std::cout << desc << std::endl;
        return std::nullopt;
    }

    po::notify(vm);

    args.config_file = vm["config-file"s].as<std::string>();
    args.www_root = vm["www-root"s].as<std::string>();

    if (vm.contains("tick-period"s)) {
        args.tick_period = vm["tick-period"s].as<int>();
    }

    args.randomize_spawn_points = vm.contains("randomize-spawn-points"s);

    if (vm.contains("state-file"s)) {
        args.state_file = fs::path{vm["state-file"s].as<std::string>()};
    }

    if (vm.contains("save-state-period"s)) {
        args.save_state_period = vm["save-state-period"s].as<int>();
    }

    return args;
}

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
    try {
        InitLogging();

        const auto args = ParseCommandLine(argc, argv);

        if (!args) {
            return EXIT_SUCCESS;
        }

        auto load_result = json_loader::LoadGame(args->config_file);
        auto game = std::move(load_result.game);
        auto extra_data = std::move(load_result.extra_data);

        game.SetRandomizeSpawnPoints(args->randomize_spawn_points);

        app::Players players;
        app::PlayerTokens player_tokens;

        if (args->state_file && fs::exists(*args->state_file)) {
            try {
                state_serialization::LoadState(
                    game,
                    players,
                    player_tokens,
                    *args->state_file
                );

                LogInfo("state loaded", json::object{
                    {"file", args->state_file->string()}
                });

            } catch (const std::exception& ex) {
                LogInfo("state load failed", json::object{
                    {"file", args->state_file->string()},
                    {"exception", ex.what()}
                });

                return EXIT_FAILURE;
            }
        }

        std::unique_ptr<state_serialization::StateSaver> state_saver;

        if (args->state_file) {
            std::optional<std::chrono::milliseconds> save_period;

            if (args->save_state_period) {
                save_period = std::chrono::milliseconds{*args->save_state_period};
            }

            state_saver = std::make_unique<state_serialization::StateSaver>(
                game,
                players,
                player_tokens,
                *args->state_file,
                save_period
            );
        }

        const unsigned num_threads = std::max(1u, std::thread::hardware_concurrency());
        net::io_context ioc(static_cast<int>(num_threads));

        auto api_strand = net::make_strand(ioc);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                ioc.stop();
            }
        });

        const bool manual_tick_enabled = !args->tick_period.has_value();

        http_handler::RequestHandler handler{
            game,
            players,
            player_tokens,
            extra_data,
            args->www_root,
            manual_tick_enabled,
            [&state_saver](std::chrono::milliseconds delta) {
                if (state_saver) {
                    state_saver->OnTick(delta);
                }
            }
        };

        std::shared_ptr<Ticker> ticker;

        if (args->tick_period) {
            ticker = std::make_shared<Ticker>(
                api_strand,
                std::chrono::milliseconds{*args->tick_period},
                [&game, &state_saver](std::chrono::milliseconds delta) {
                    const double delta_seconds = static_cast<double>(delta.count()) / 1000.0;
                    game.Update(delta_seconds);

                    if (state_saver) {
                        state_saver->OnTick(delta);
                    }
                }
            );

            ticker->Start();
        }

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

        if (state_saver) {
            state_saver->Save();

            LogInfo("state saved", json::object{
                {"file", args->state_file->string()}
            });
        }

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
