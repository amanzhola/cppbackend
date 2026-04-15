#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <chrono>
#include <string>

namespace net = boost::asio;
using namespace std::literals;

// ===== Имитация Mailer =====
class Mailer {
public:
    using Callback = std::function<void(bool)>;

    Mailer(net::io_context& io)
        : timer_(io) {}

    void SendMailAsync(Callback cb) {
        std::cout << "Mailer: sending mail...\n";

        timer_.expires_after(2s);

        timer_.async_wait([cb](boost::system::error_code ec) {
            if (ec) return;
            std::cout << "Mailer: mail sent\n";
            cb(true);
        });
    }

private:
    net::steady_timer timer_;
};

// ===== Имитация Label =====
class Label {
public:
    void SetText(const std::string& text) {
        std::cout << "UI: " << text << std::endl;
    }
};

// ===== MailWidget =====
class MailWidget : public std::enable_shared_from_this<MailWidget> {
public:
    MailWidget(Mailer& mailer)
        : mailer_(mailer) {}

    void OnSendMailButtonClick() {
        status_label_.SetText("Sending mail");

        mailer_.SendMailAsync(
            [weak_self = weak_from_this()](bool success) {

                if (auto self = weak_self.lock()) {
                    self->status_label_.SetText(
                        success ? "Mail sent!" : "Error");
                } else {
                    std::cout << "Widget already destroyed\n";
                }
            });
    }

private:
    Mailer& mailer_;
    Label status_label_;
};

// ===== main =====
int main() {
    net::io_context io;

    Mailer mailer(io);

    {
        auto widget = std::make_shared<MailWidget>(mailer);

        widget->OnSendMailButtonClick();

        // 💥 имитируем закрытие окна
        std::cout << "User closes window\n";
    }

    io.run();

    std::cout << "Done\n";
}
