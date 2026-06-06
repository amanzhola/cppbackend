#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

struct GameState {
    uint32_t tick_count = 0;
    uint32_t player_score = 0;
    std::string player_token;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & tick_count;
        ar & player_score;
        ar & player_token;
    }
};

void SaveStateSafely(const GameState& state, const fs::path& target_file) {
    const fs::path temp_file = target_file.string() + ".tmp";

    {
        std::ofstream out(temp_file);

        if (!out) {
            throw std::runtime_error("Can not open temporary save file");
        }

        boost::archive::text_oarchive archive(out);

        archive << state;
    }

    fs::rename(temp_file, target_file);
}

GameState LoadState(const fs::path& target_file) {
    std::ifstream in(target_file);

    if (!in) {
        throw std::runtime_error("Can not open save file");
    }

    boost::archive::text_iarchive archive(in);

    GameState state;

    archive >> state;

    return state;
}

int main() {
    const fs::path save_file = "game_state.txt";

    GameState state{
        42,
        1000,
        "secret-player-token"
    };

    SaveStateSafely(state, save_file);

    GameState loaded_state = LoadState(save_file);

    std::cout << "tick_count: " << loaded_state.tick_count << std::endl;
    std::cout << "player_score: " << loaded_state.player_score << std::endl;
    std::cout << "player_token: " << loaded_state.player_token << std::endl;
}
