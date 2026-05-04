#pragma once

#include "../model/dog.h"
#include "../model/game_session.h"

#include <string>
#include <unordered_map>

namespace app {

class Player {
public:
    Player(std::string token, model::GameSession& session, model::Dog::Id dog_id)
        : token_{std::move(token)}
        , session_{session}
        , dog_id_{dog_id} {
    }

    const std::string& GetToken() const noexcept {
        return token_;
    }

    model::GameSession& GetSession() const noexcept {
        return session_;
    }

    model::Dog::Id GetDogId() const noexcept {
        return dog_id_;
    }

private:
    std::string token_;
    model::GameSession& session_;
    model::Dog::Id dog_id_;
};

class Players {
public:
    Player& Add(std::string token, model::GameSession& session, model::Dog::Id dog_id) {
        auto [it, inserted] = players_.emplace(
            token,
            Player{token, session, dog_id}
        );
        return it->second;
    }

    Player* FindByToken(const std::string& token) {
        if (auto it = players_.find(token); it != players_.end()) {
            return &it->second;
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, Player> players_;
};

}  // namespace app
