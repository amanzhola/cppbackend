#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <memory>
#include <functional>
#include <sstream>
#include <chrono>
#include <syncstream>

#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>

using namespace std::literals;
using namespace std::chrono;

namespace net = boost::asio;
namespace sys = boost::system;

// ---------------------------
// Учебный пример: как нельзя
// ---------------------------
class Bad {
public:
    void SomeMethod() {
        // ТАК ДЕЛАТЬ НЕЛЬЗЯ:
        // создаётся новый shared_ptr, который не знает о других владельцах
        std::shared_ptr<Bad> self(this);
        (void)self;
    }
};

// ---------------------------
// Учебный пример: как правильно
// ---------------------------
class Good : public std::enable_shared_from_this<Good> {
public:
    void SomeMethod() {
        std::shared_ptr<Good> self = shared_from_this();
        std::weak_ptr<Good> weak_self = weak_from_this();
        (void)self;
        (void)weak_self;
    }
};

class Hamburger {
public:
    [[nodiscard]] bool IsCutletRoasted() const {
        return cutlet_roasted_;
    }

    void SetCutletRoasted() {
        if (IsCutletRoasted()) {
            throw std::logic_error("Cutlet has been roasted already"s);
        }
        cutlet_roasted_ = true;
    }

    [[nodiscard]] bool HasOnion() const {
        return has_onion_;
    }

    void AddOnion() {
        if (IsPacked()) {
            throw std::logic_error("Hamburger has been packed already"s);
        }
        AssureCutletRoasted();
        has_onion_ = true;
    }

    [[nodiscard]] bool IsPacked() const {
        return is_packed_;
    }

    void Pack() {
        AssureCutletRoasted();
        is_packed_ = true;
    }

private:
    void AssureCutletRoasted() const {
        if (!cutlet_roasted_) {
            throw std::logic_error("Cutlet has not been roasted yet"s);
        }
    }

    bool cutlet_roasted_ = false;
    bool has_onion_ = false;
    bool is_packed_ = false;
};

std::ostream& operator<<(std::ostream& os, const Hamburger& h) {
    return os << "Hamburger: "sv
              << (h.IsCutletRoasted() ? "roasted cutlet"sv : "raw cutlet"sv)
              << (h.HasOnion() ? ", onion"sv : ""sv)
              << (h.IsPacked() ? ", packed"sv : ", not packed"sv);
}

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

using OrderHandler = std::function<void(sys::error_code ec, int id, Hamburger* hamburger)>;
using Timer = net::steady_timer;

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
        roast_timer_.expires_after(1s);
        roast_timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
            self->OnRoasted(ec);
        });
    }

    void MarinadeOnion() {
        logger_.LogMessage("Start marinading onion"sv);
        marinade_timer_.expires_after(2s);
        marinade_timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
            self->OnOnionMarinaded(ec);
        });
    }

    void OnRoasted(sys::error_code ec) {
        if (ec) {
            logger_.LogMessage("Roast error: "s + ec.what());
        } else {
            logger_.LogMessage("Cutlet has been roasted."sv);
            hamburger_.SetCutletRoasted();
        }
        CheckReadiness(ec);
    }

    void OnOnionMarinaded(sys::error_code ec) {
        if (ec) {
            logger_.LogMessage("Marinade onion error: "s + ec.what());
        } else {
            logger_.LogMessage("Onion has been marinaded."sv);
            onion_marinaded_ = true;
        }
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
        return hamburger_.IsCutletRoasted()
            && onion_marinaded_
            && !hamburger_.HasOnion();
    }

    [[nodiscard]] bool IsReadyToPack() const {
        return hamburger_.IsCutletRoasted()
            && (!with_onion_ || hamburger_.HasOnion())
            && !hamburger_.IsPacked();
    }

    void Pack() {
        logger_.LogMessage("Packing"sv);

        auto start = steady_clock::now();
        while (steady_clock::now() - start < 500ms) {
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

    Timer roast_timer_{io_};
    Timer marinade_timer_{io_};

    Hamburger hamburger_;
    bool onion_marinaded_ = false;
    bool delivered_ = false;
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
    net::io_context io;

    Restaurant restaurant{io};
    Logger logger{"main"s};

    auto print_result = [&logger](sys::error_code ec, int order_id, Hamburger* hamburger) {
        std::ostringstream os;
        if (ec) {
            os << "Order " << order_id << " failed: " << ec.what();
            logger.LogMessage(os.str());
            return;
        }
        os << "Order " << order_id << " is ready. " << *hamburger;
        logger.LogMessage(os.str());
    };

    for (int i = 0; i < 4; ++i) {
        restaurant.MakeHamburger(i % 2 == 0, print_result);
    }

    io.run();
}
