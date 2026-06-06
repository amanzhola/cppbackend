#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/signals2.hpp>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace sig = boost::signals2;

using milliseconds = std::chrono::milliseconds;
using namespace std::literals;

struct GameState {
    uint32_t tick_count = 0;
    uint32_t player_score = 0;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & tick_count;
        ar & player_score;
    }
};

class Application {
public:
    using TickSignal = sig::signal<void(milliseconds delta)>;

    [[nodiscard]] sig::connection DoOnTick(const TickSignal::slot_type& handler) {
        return tick_signal_.connect(handler);
    }

    void Tick(milliseconds delta) {
        state_.tick_count += 1;
        state_.player_score += 10;

        std::cout << "Tick: " << state_.tick_count
                  << ", score: " << state_.player_score
                  << ", delta: " << delta.count() << "ms"
                  << std::endl;

        tick_signal_(delta);
    }

    const GameState& GetState() const {
        return state_;
    }

private:
    GameState state_;
    TickSignal tick_signal_;
};

class SerializingListener {
public:
    SerializingListener(const Application& app,
                        std::string save_file,
                        milliseconds save_period)
        : app_(app)
        , save_file_(std::move(save_file))
        , save_period_(save_period) {
    }

    void OnTick(milliseconds delta) {
        time_since_save_ += delta;

        if (time_since_save_ >= save_period_) {
            Save();
            time_since_save_ = 0ms;
        }
    }

    void Save() const {
        std::ofstream out(save_file_);

        boost::archive::text_oarchive archive(out);

        archive << app_.GetState();

        std::cout << "Saved state to " << save_file_ << std::endl;
    }

private:
    const Application& app_;
    std::string save_file_;
    milliseconds save_period_;
    milliseconds time_since_save_{0};
};

int main() {
    Application app;

    SerializingListener listener{
        app,
        "game_state.txt",
        3500ms
    };

    sig::scoped_connection connection = app.DoOnTick(
        [&listener](milliseconds delta) {
            listener.OnTick(delta);
        }
    );

    app.Tick(1000ms);
    app.Tick(1000ms);
    app.Tick(1000ms);
    app.Tick(750ms);
    app.Tick(1000ms);
    app.Tick(1000ms);
    app.Tick(1000ms);
    app.Tick(1250ms);

    listener.Save();
}

