#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>

namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::chrono;
using namespace std::literals;
using Timer = net::steady_timer;

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);

    std::vector<std::jthread> workers;
    workers.reserve(n - 1);

    while (--n) {
        workers.emplace_back(fn);
    }

    fn();
}

class ThreadChecker {
public:
    explicit ThreadChecker(std::atomic_int& counter)
        : counter_{counter} {
    }

    ThreadChecker(const ThreadChecker&) = delete;
    ThreadChecker& operator=(const ThreadChecker&) = delete;

    ~ThreadChecker() {
        assert(expected_counter_ == counter_);
    }

private:
    std::atomic_int& counter_;
    int expected_counter_ = ++counter_;
};

class Logger {
public:
    explicit Logger(std::string id)
        : id_(std::move(id)) {
    }

    void LogMessage(std::string_view message) const {
        std::osyncstream os{std::cout};
        os << id_ << "> ["
           << duration<double>(steady_clock::now() - start_time_).count()
           << "s] " << message << std::endl;
    }

private:
    std::string id_;
    steady_clock::time_point start_time_{steady_clock::now()};
};

class Hamburger {
public:
    [[nodiscard]] bool IsCutletRoasted() const {
        return cutlet_roasted_;
    }

    void SetCutletRoasted() {
        if (cutlet_roasted_) {
            throw std::logic_error("Cutlet has been roasted already"s);
        }
        cutlet_roasted_ = true;
    }

    [[nodiscard]] bool HasOnion() const {
        return has_onion_;
    }

    void AddOnion() {
        if (is_packed_) {
            throw std::logic_error("Hamburger has been packed already"s);
        }
        if (!cutlet_roasted_) {
            throw std::logic_error("Cutlet has not been roasted yet"s);
        }
        has_onion_ = true;
    }

    [[nodiscard]] bool IsPacked() const {
        return is_packed_;
    }

    void Pack() {
        if (!cutlet_roasted_) {
            throw std::logic_error("Cutlet has not been roasted yet"s);
        }
        is_packed_ = true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hamburger& h) {
        return os << "Hamburger: "
                  << (h.IsCutletRoasted() ? "roasted cutlet" : "raw cutlet")
                  << (h.HasOnion() ? ", onion" : "")
                  << (h.IsPacked() ? ", packed" : ", not packed");
    }

private:
    bool cutlet_roasted_ = false;
    bool has_onion_ = false;
    bool is_packed_ = false;
};

using OrderHandler = std::function<void(sys::error_code, int, Hamburger*)>;

class Order : public std::enable_shared_from_this<Order> {
public:
    Order(net::io_context& io, int id, bool with_onion, OrderHandler handler)
        : io_(io)
        , id_(id)
        , with_onion_(with_onion)
        , handler_(std::move(handler))
        , logger_(std::to_string(id)) {
    }

    void Execute() {
        logger_.LogMessage("Order has been started."sv);
        RoastCutlet();
        if (with_onion_) {
            MarinadeOnion();
        }
    }

private:
    void RoastCutlet() {
        logger_.LogMessage("Start roasting cutlet"sv);
        roast_timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
            self->OnRoasted(ec);
        });
    }

    void MarinadeOnion() {
        logger_.LogMessage("Start marinading onion"sv);
        marinade_timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
            self->OnOnionMarinaded(ec);
        });
    }

    void OnRoasted(sys::error_code ec) {
        ThreadChecker checker{counter_};

        if (ec) {
            logger_.LogMessage("Roast error"sv);
        } else {
            logger_.LogMessage("Cutlet has been roasted."sv);
            hamburger_.SetCutletRoasted();
        }

        // Увеличиваем вероятность пересечения потоков
        std::this_thread::sleep_for(2ms);

        CheckReadiness(ec);
    }

    void OnOnionMarinaded(sys::error_code ec) {
        ThreadChecker checker{counter_};

        if (ec) {
            logger_.LogMessage("Marinade onion error"sv);
        } else {
            logger_.LogMessage("Onion has been marinaded."sv);
            onion_marinaded_ = true;
        }

        // Увеличиваем вероятность пересечения потоков
        std::this_thread::sleep_for(2ms);

        CheckReadiness(ec);
    }

    void CheckReadiness(sys::error_code ec) {
        if (delivered_) {
            return;
        }
        if (ec) {
            return Deliver(ec);
        }
        if (CanAddOnion()) {
            logger_.LogMessage("Add onion"sv);
            hamburger_.AddOnion();
        }
        if (IsReadyToPack()) {
            Pack();
        }
    }

    void Deliver(sys::error_code ec) {
        delivered_ = true;
        handler_(ec, id_, ec ? nullptr : &hamburger_);
    }

    [[nodiscard]] bool CanAddOnion() const {
        return hamburger_.IsCutletRoasted() && onion_marinaded_ && !hamburger_.HasOnion();
    }

    [[nodiscard]] bool IsReadyToPack() const {
        return hamburger_.IsCutletRoasted()
            && (!with_onion_ || hamburger_.HasOnion())
            && !hamburger_.IsPacked();
    }

    void Pack() {
        logger_.LogMessage("Packing"sv);

        auto start = steady_clock::now();
        while (steady_clock::now() - start < 1ms) {
        }

        hamburger_.Pack();
        logger_.LogMessage("Packed"sv);

        Deliver({});
    }

private:
    net::io_context& io_;
    int id_;
    bool with_onion_;
    OrderHandler handler_;
    Logger logger_;

    // Временно уменьшаем таймеры до 1ms, чтобы повысить вероятность гонки
    Timer roast_timer_{io_, 1ms};
    Timer marinade_timer_{io_, 1ms};

    Hamburger hamburger_;
    bool onion_marinaded_ = false;
    bool delivered_ = false;

    // Счётчик для ThreadChecker
    std::atomic_int counter_{0};
};

class Restaurant {
public:
    explicit Restaurant(net::io_context& io)
        : io_(io) {
    }

    int MakeHamburger(bool with_onion, OrderHandler handler) {
        const int order_id = ++next_order_id_;
        std::make_shared<Order>(io_, order_id, with_onion, std::move(handler))->Execute();
        return order_id;
    }

private:
    net::io_context& io_;
    int next_order_id_ = 0;
};

int main() {
    const unsigned num_workers = 4;
    net::io_context io(num_workers);

    Restaurant restaurant{io};
    Logger logger{"main"s};

    auto handler = [&logger](sys::error_code ec, int id, Hamburger* h) {
        std::ostringstream os;
        if (ec) {
            os << "Order " << id << " failed";
        } else if (h) {
            os << "Order " << id << " completed: " << *h;
        }
        logger.LogMessage(os.str());
    };

    // 16 заказов: 8 с луком, 8 без лука
    for (int i = 0; i < 16; ++i) {
        restaurant.MakeHamburger(i % 2 == 0, handler);
    }

    RunWorkers(num_workers, [&io] {
        io.run();
    });
}
