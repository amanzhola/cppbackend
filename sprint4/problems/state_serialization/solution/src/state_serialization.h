#pragma once

#include "model.h"
#include "model_serialization.h"
#include "players.h"
#include "player_tokens.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace state_serialization {

namespace fs = std::filesystem;
using milliseconds = std::chrono::milliseconds;

struct PlayerRepr {
    app::Player::Id id = 0;
    std::string name;
    std::string map_id;
    model::Dog::Id dog_id = 0;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & id;
        ar & name;
        ar & map_id;
        ar & dog_id;
    }
};

struct TokenRepr {
    std::string token;
    app::Player::Id player_id = 0;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & token;
        ar & player_id;
    }
};

class GameStateRepr {
public:
    GameStateRepr() = default;

    GameStateRepr(const model::Game& game,
                  const app::Players& players,
                  const app::PlayerTokens& tokens) {
        for (const model::GameSession& session : game.GetSessions()) {
            sessions.emplace_back(session);
        }

        for (const auto& [id, player] : players.GetAll()) {
            this->players.push_back(PlayerRepr{
                .id = player.GetId(),
                .name = player.GetName(),
                .map_id = *player.GetSession().GetMap().GetId(),
                .dog_id = player.GetDogId()
            });
        }

        for (const auto& [token, player_id] : tokens.Export()) {
            this->tokens.push_back(TokenRepr{
                .token = token,
                .player_id = player_id
            });
        }
    }

    void Restore(model::Game& game,
                 app::Players& players,
                 app::PlayerTokens& tokens) const {
        for (const auto& session : sessions) {
            session.Restore(game);
        }

        for (const PlayerRepr& player_repr : this->players) {
            const model::Map* map = game.FindMap(model::Map::Id{player_repr.map_id});

            if (!map) {
                throw std::runtime_error("Map not found while restoring player");
            }

            model::GameSession& session = game.FindOrCreateSession(*map);

            if (!session.FindDog(player_repr.dog_id)) {
                throw std::runtime_error("Dog not found while restoring player");
            }

            players.AddRestored(
                player_repr.id,
                player_repr.name,
                session,
                player_repr.dog_id
            );
        }

        for (const TokenRepr& token_repr : this->tokens) {
            app::Player* player = players.FindById(token_repr.player_id);

            if (!player) {
                throw std::runtime_error("Player not found while restoring token");
            }

            tokens.AddRestoredToken(token_repr.token, *player);
        }
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & sessions;
        ar & players;
        ar & tokens;
    }

private:
    std::vector<serialization::SessionRepr> sessions;
    std::vector<PlayerRepr> players;
    std::vector<TokenRepr> tokens;
};

inline void SaveStateSafely(const model::Game& game,
                            const app::Players& players,
                            const app::PlayerTokens& tokens,
                            const fs::path& target_file) {
    const fs::path temp_file = target_file.string() + ".tmp";

    {
        std::ofstream out(temp_file);

        if (!out) {
            throw std::runtime_error("Can not open temporary state file");
        }

        boost::archive::text_oarchive archive(out);
        GameStateRepr state{game, players, tokens};
        archive << state;
    }

    fs::rename(temp_file, target_file);
}

inline void LoadState(model::Game& game,
                      app::Players& players,
                      app::PlayerTokens& tokens,
                      const fs::path& state_file) {
    std::ifstream in(state_file);

    if (!in) {
        throw std::runtime_error("Can not open state file");
    }

    boost::archive::text_iarchive archive(in);

    GameStateRepr state;
    archive >> state;

    state.Restore(game, players, tokens);
}

class StateSaver {
public:
    StateSaver(model::Game& game,
               app::Players& players,
               app::PlayerTokens& tokens,
               fs::path state_file,
               std::optional<milliseconds> save_period)
        : game_(game)
        , players_(players)
        , tokens_(tokens)
        , state_file_(std::move(state_file))
        , save_period_(save_period) {
    }

    void OnTick(milliseconds delta) {
        if (!save_period_) {
            return;
        }

        time_since_save_ += delta;

        if (time_since_save_ >= *save_period_) {
            Save();
            time_since_save_ = milliseconds{0};
        }
    }

    void Save() const {
        SaveStateSafely(game_, players_, tokens_, state_file_);
    }

private:
    model::Game& game_;
    app::Players& players_;
    app::PlayerTokens& tokens_;
    fs::path state_file_;
    std::optional<milliseconds> save_period_;
    milliseconds time_since_save_{0};
};

}  // namespace state_serialization
