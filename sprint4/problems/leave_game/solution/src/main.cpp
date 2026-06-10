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
#include "db.h"

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
	const auto dog_retirement_time = load_result.dog_retirement_time;

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

	const char* game_db_url = std::getenv("GAME_DB_URL");

	if (!game_db_url) {
    		throw std::runtime_error("GAME_DB_URL is not specified");
	}

	db::ConnectionPool connection_pool{
    		num_threads,
    		[game_db_url] {
        		return std::make_shared<pqxx::connection>(game_db_url);
    		}
	};

	db::RetiredPlayersRepository records_repository{connection_pool};
	records_repository.Init();

        net::io_context ioc(static_cast<int>(num_threads));

        auto api_strand = net::make_strand(ioc);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                ioc.stop();
            }
        });

        const bool manual_tick_enabled = !args->tick_period.has_value();

	auto retire_inactive_players = [&players,
                                &player_tokens,
                                &records_repository,
                                dog_retirement_time] {
    	     std::vector<app::Player::Id> players_to_remove;

    	     for (const auto& [player_id, player] : players.GetAll()) {
        	model::GameSession& session = player.GetSession();
        	model::Dog* dog = session.FindDog(player.GetDogId());

             if (!dog) {
            	players_to_remove.push_back(player_id);
            	continue;
             }

             if (dog->GetIdleTime() >= dog_retirement_time) {
            		records_repository.Save(
                		dog->GetName(),
                		dog->GetScore(),
                		dog->GetPlayTime().count()
            		);

            		session.RemoveDog(dog->GetId());
            		players_to_remove.push_back(player_id);
        	}
    	    }

    	    for (app::Player::Id player_id : players_to_remove) {
        		player_tokens.RemovePlayer(player_id);
        		players.Remove(player_id);
    	   }
	};

        http_handler::RequestHandler handler{
            game,
            players,
            player_tokens,
            extra_data,
            args->www_root,
            manual_tick_enabled,
            [&state_saver, &retire_inactive_players](std::chrono::milliseconds delta) {
    		retire_inactive_players();

    	  	if (state_saver) {
        	 state_saver->OnTick(delta);
    	  	}
	    },
	    [&records_repository](size_t start, size_t max_items) {
    	     json::array result;

    	     for (const db::Record& record : records_repository.GetRecords(start, max_items)) {
        	json::object obj;
        	obj["name"] = record.name;
        	obj["score"] = record.score;
        	obj["playTime"] = static_cast<double>(record.play_time_ms) / 1000.0;
        	result.emplace_back(std::move(obj));
    	     }

    	     return result;
	  }
        };

        std::shared_ptr<Ticker> ticker;

        if (args->tick_period) {
            ticker = std::make_shared<Ticker>(
                api_strand,
                std::chrono::milliseconds{*args->tick_period},
                [&game, &state_saver, &retire_inactive_players](std::chrono::milliseconds delta) {
                    const double delta_seconds = static_cast<double>(delta.count()) / 1000.0;
                    game.Update(delta_seconds);
		    retire_inactive_players();

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
        LogInfo("server exited", json::object{
            {"code", EXIT_FAILURE},
            {"exception", ex.what()}
        });

        return EXIT_FAILURE;
    }
}
