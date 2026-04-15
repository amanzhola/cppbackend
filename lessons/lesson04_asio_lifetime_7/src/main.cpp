#include <boost/asio.hpp>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include <chrono>
#include <string_view>

namespace net = boost::asio;
namespace chrono = std::chrono;
using namespace std::literals;
using boost::system::error_code;

class Robot : public std::enable_shared_from_this<Robot> {
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
        std::cout << id_ << "> Walk for " << t.count() << "sec\n";

        timer_.expires_after(chrono::duration_cast<Duration>(t));

        timer_.async_wait(
            [distance,
             cb = std::forward<Callback>(cb),
             self = shared_from_this()](error_code ec) mutable {

                if (ec) {
                    throw std::runtime_error(ec.message());
                }

                self->walk_distance_ += distance;
                std::cout << self->id_
                          << "> Walked distance: "
                          << self->walk_distance_
                          << "m\n";

                cb();
            });
    }

    template <typename Callback>
    void Rotate(int angle, Callback&& cb) {
        const auto t = 1s * std::abs(angle) / ROTATION_SPEED;
        std::cout << id_ << "> Rotate for " << t.count() << "sec\n";

        timer_.expires_after(chrono::duration_cast<Duration>(t));

        timer_.async_wait(
            [angle,
             cb = std::forward<Callback>(cb),
             self = shared_from_this()](error_code ec) mutable {

                if (ec) {
                    throw std::runtime_error(ec.message());
                }

                self->angle_ = (self->angle_ + angle) % 360;
                std::cout << self->id_
                          << "> Rotation angle: "
                          << self->angle_
                          << "deg.\n";

                cb();
            });
    }

private:
    net::steady_timer timer_;
    int id_;
    int angle_ = 0;
    int walk_distance_ = 0;
};

void RunRobots(net::io_context& io) {
    Robot r1(io, 1);
    Robot r2(io, 2);

    r1.Rotate(60, [&r1] {
        r1.Walk(4, [] {});
    });

    r2.Walk(2, [&r2] {
        r2.Walk(3, [] {});
    });
}

int main() {
    net::io_context io;

    for (;;) {
        try {
            RunRobots(io);
            io.run();
            break;
        } catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << '\n';
            break;
        }
    }

    std::cout << "Done\n";
}
