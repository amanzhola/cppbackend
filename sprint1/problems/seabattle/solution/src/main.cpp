#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    std::array<char, sz> buf{};
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return std::string(buf.data(), sz);
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field)
        , other_field_(SeabattleField::State::UNKNOWN) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        bool my_turn = my_initiative;

        while (!IsGameEnded()) {
            PrintFields();

            if (my_turn) {
                std::pair<int, int> move{};
                std::string move_str;

                while (true) {
                    std::cout << "Your move (e.g. B6): ";
                    std::getline(std::cin, move_str);

                    auto parsed = ParseMove(move_str);
                    if (!parsed) {
                        std::cout << "Invalid move. Use A1..H8\n";
                        continue;
                    }

                    move = *parsed;
                    break;
                }

                if (!SendMove(socket, move)) {
                    std::cout << "Failed to send move\n";
                    return;
                }

                auto result = ReadResult(socket);
                if (!result) {
                    std::cout << "Failed to read shot result\n";
                    return;
                }

                auto [x, y] = move;

                switch (*result) {
                    case SeabattleField::ShotResult::MISS:
                        std::cout << "Miss\n";
                        other_field_.MarkMiss(x, y);
                        my_turn = false;
                        break;
                    case SeabattleField::ShotResult::HIT:
                        std::cout << "Hit\n";
                        other_field_.MarkHit(x, y);
                        my_turn = true;
                        break;
                    case SeabattleField::ShotResult::KILL:
                        std::cout << "Kill\n";
                        other_field_.MarkKill(x, y);
                        my_turn = true;
                        break;
                }
            } else {
                auto move = ReadMove(socket);
                if (!move) {
                    std::cout << "Failed to read opponent move\n";
                    return;
                }

                auto [x, y] = *move;
                std::cout << "Opponent move: " << MoveToString(*move) << std::endl;

                auto result = my_field_.Shoot(x, y);

                if (!SendResult(socket, result)) {
                    std::cout << "Failed to send shot result\n";
                    return;
                }

                switch (result) {
                    case SeabattleField::ShotResult::MISS:
                        std::cout << "Opponent missed\n";
                        my_turn = true;
                        break;
                    case SeabattleField::ShotResult::HIT:
                        std::cout << "Opponent hit your ship\n";
                        my_turn = false;
                        break;
                    case SeabattleField::ShotResult::KILL:
                        std::cout << "Opponent killed your ship\n";
                        my_turn = false;
                        break;
                }
            }
        }

        PrintFields();

        if (my_field_.IsLoser()) {
            std::cout << "You lose\n";
        } else {
            std::cout << "You win\n";
        }
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) {
            return std::nullopt;
        }

        int y = sv[0] - 'A';  // буква = строка
        int x = sv[1] - '1';  // цифра = столбец

        if (y < 0 || y >= static_cast<int>(SeabattleField::field_size)) {
            return std::nullopt;
        }
        if (x < 0 || x >= static_cast<int>(SeabattleField::field_size)) {
            return std::nullopt;
        }

        return {{x, y}};
    }

    static std::string MoveToString(std::pair<int, int> move) {
        int x = move.first;
        int y = move.second;

        char buff[] = {
            static_cast<char>('A' + y),
            static_cast<char>('1' + x)
        };
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    std::optional<std::pair<int, int>> ReadMove(tcp::socket& socket) {
        auto raw = ReadExact<2>(socket);
        if (!raw) {
            return std::nullopt;
        }
        return ParseMove(*raw);
    }

    std::optional<SeabattleField::ShotResult> ReadResult(tcp::socket& socket) {
        auto raw = ReadExact<1>(socket);
        if (!raw) {
            return std::nullopt;
        }
        return static_cast<SeabattleField::ShotResult>((*raw)[0]);
    }

    bool SendMove(tcp::socket& socket, std::pair<int, int> move) {
        return WriteExact(socket, MoveToString(move));
    }

    bool SendResult(tcp::socket& socket, SeabattleField::ShotResult result) {
        char c = static_cast<char>(result);
        return WriteExact(socket, std::string_view(&c, 1));
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    net::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    tcp::socket socket(io_context);

    std::cout << "Waiting for client on port " << port << "...\n";
    acceptor.accept(socket);
    std::cout << "Client connected\n";

    SeabattleAgent agent(field);
    agent.StartGame(socket, false);
}

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    net::io_context io_context;
    tcp::socket socket(io_context);

    socket.connect(tcp::endpoint(net::ip::make_address(ip_str), port));
    std::cout << "Connected to server\n";

    SeabattleAgent agent(field);
    agent.StartGame(socket, true);
}

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField field = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(field, static_cast<unsigned short>(std::stoi(argv[2])));
    } else {
        StartClient(field, argv[2], static_cast<unsigned short>(std::stoi(argv[3])));
    }

    return 0;
}
