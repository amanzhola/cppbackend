#include <boost/signals2.hpp>

#include <chrono>
#include <iostream>

namespace sig = boost::signals2;

using milliseconds = std::chrono::milliseconds;
using namespace std::literals;

class Application {
public:
    using TickSignal = sig::signal<void(milliseconds delta)>;

    [[nodiscard]] sig::connection DoOnTick(const TickSignal::slot_type& handler) {
        return tick_signal_.connect(handler);
    }

    void Tick(milliseconds delta) {
        tick_signal_(delta);
    }

private:
    TickSignal tick_signal_;
};

int main() {
    Application app;

    app.Tick(30ms);

    sig::scoped_connection conn1 = app.DoOnTick([total = 0ms](milliseconds delta) mutable {
        total += delta;

        std::cout << "Tick! Delta: "
                  << delta.count()
                  << "ms, Total: "
                  << total.count()
                  << "ms"
                  << std::endl;
    });

    app.Tick(15ms);

    {
        sig::scoped_connection conn2 = app.DoOnTick([]([[maybe_unused]] milliseconds delta) {
            std::cout << "⌚" << std::endl;
        });

        app.Tick(20ms);
        app.Tick(50ms);
    }

    std::cout << "---" << std::endl;

    app.Tick(40ms);
    app.Tick(10ms);
}
