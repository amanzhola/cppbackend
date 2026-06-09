#pragma once

#include <chrono>
#include <filesystem>

#include "extra_data.h"
#include "model.h"

namespace json_loader {

struct GameLoadResult {
    model::Game game;
    extra_data::ExtraData extra_data;
    std::chrono::milliseconds dog_retirement_time{60000};
};

GameLoadResult LoadGame(const std::filesystem::path& json_path);

}  // namespace json_loader
