#include <boost/asio.hpp>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <chrono>
#include <string_view>

namespace net = boost::asio;
namespace chrono = std::chrono;
using namespace std::literals;
using boost::system::error_code;

class Robot {
public:
    using Duration = net::steady_timer::duration;

    constexpr static double SPEED = 2;
    constexpr static double ROTATION_SPEED = 30;

    Robot(net::io_context& io, int id)
        : timer_{io}
        , id_{id} {
    }

    template <typename Callback>
    void Walk(int distance, Callback&& cb) {
        const auto t = 1s * distance / SPEED;
        std::cout << id_ << "> Walk for "sv << t.count() << "s\n"sv;

        timer_.expires_after(chrono::duration_cast<Duration>(t));

        timer_.async_wait([distance, cb = std::forward<Callback>(cb), this](error_code ec) mutable {
            if (ec) {
                throw std::runtime_error(ec.message());
            }
            walk_distance_ += distance;
            std::cout << id_ << "> Walked distance: "sv << walk_distance_ << "m\n"sv;
            cb();
        });
    }

    template <typename Callback>
    void Rotate(int angle, Callback&& cb) {
        const auto t = 1s * std::abs(angle) / ROTATION_SPEED;
        std::cout << id_ << "> Start rotating by "sv << angle << "deg.\n"sv;

        timer_.expires_after(chrono::duration_cast<Duration>(t));

        timer_.async_wait([angle, cb = std::forward<Callback>(cb), this](error_code ec) mutable {
            if (ec) {
                throw std::runtime_error(ec.message());
            }
            angle_ = (angle_ + angle) % 360;
            std::cout << id_ << "> Rotation angle: "sv << angle_ << "deg.\n"sv;
            cb();
        });
    }

private:
    net::steady_timer timer_;
    int id_;
    int angle_ = 0;
    int walk_distance_ = 0;
};

int main() {
    net::io_context io;

    Robot r1(io, 1);
    Robot r2(io, 2);

    const auto r1_start = chrono::steady_clock::now();

    r1.Rotate(60, [&r1, r1_start] {
        r1.Walk(4, [r1_start] {
            const auto r1_finish = chrono::steady_clock::now();
            const auto r1_time = chrono::duration_cast<chrono::milliseconds>(r1_finish - r1_start);
            std::cout << "Robot r1 finished in "sv << r1_time.count() / 1000.0 << "s\n"sv;
        });
    });

    r2.Walk(2, [&r2] {
        r2.Walk(3, [] {});
    });

    for (;;) {
        try {
            io.run();
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }

    std::cout << "Done\n"sv;
}
