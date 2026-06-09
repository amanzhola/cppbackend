#pragma once

#include "players.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace app {

class PlayerTokens {
public:
    using Token = std::string;

    Token AddPlayer(Player& player) {
        Token token = GenerateToken();
        token_to_player_[token] = &player;
        return token;
    }

    void AddRestoredToken(Token token, Player& player) {
        token_to_player_[std::move(token)] = &player;
    }

    Player* FindPlayerByToken(const Token& token) const {
        if (auto it = token_to_player_.find(token); it != token_to_player_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void RemovePlayer(Player::Id player_id) {
    for (auto it = token_to_player_.begin(); it != token_to_player_.end(); ) {
        if (it->second->GetId() == player_id) {
            it = token_to_player_.erase(it);
        } else {
            ++it;
        }
      }
   }

    std::vector<std::pair<Token, Player::Id>> Export() const {
        std::vector<std::pair<Token, Player::Id>> result;
        result.reserve(token_to_player_.size());

        for (const auto& [token, player] : token_to_player_) {
            result.emplace_back(token, player->GetId());
        }

        return result;
    }

private:
    Token GenerateToken() {
        std::ostringstream out;

        out << std::hex << std::setfill('0')
            << std::setw(16) << generator1_()
            << std::setw(16) << generator2_();

        return out.str();
    }

    std::random_device random_device_;

    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};

    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};

    std::unordered_map<Token, Player*> token_to_player_;
};

}  // namespace app
