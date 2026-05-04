#pragma once

#include "players.h"

#include <string>

namespace app {

class JoinGameUseCase {
public:
    struct Result {
        std::string auth_token;
        model::Dog::Id dog_id;
    };

    Result Join(model::GameSession& session, std::string dog_name) {
        model::Dog& dog = session.AddDog(std::move(dog_name));

        std::string token = "temporary-token-" + std::to_string(dog.GetId());

        players_.Add(token, session, dog.GetId());

        return Result{
            .auth_token = token,
            .dog_id = dog.GetId()
        };
    }

private:
    Players players_;
};

}  // namespace app
