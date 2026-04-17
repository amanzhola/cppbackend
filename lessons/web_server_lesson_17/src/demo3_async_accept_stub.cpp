// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include "common.h"

// Ядро асинхронного HTTP-сервера будет располагаться в пространстве имён http_server
namespace http_server {

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace sys = boost::system;
using namespace std::literals;

// Печать ошибок сервера
void ReportError(beast::error_code ec, std::string_view what) {
    std::cerr << what << ": "sv << ec.message() << std::endl;
}

// Простая сессия.
// Пока это не HTTP-сессия, а просто демонстрация:
// при подключении клиента отправляем строку "client connected\n"
// и закрываем соединение.
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket&& socket)
        : socket_(std::move(socket)) {
    }

    void Run() {
        std::cout << "client connected" << std::endl;
        WriteGreeting();
    }

private:
    void WriteGreeting() {
        auto self = shared_from_this();

        net::async_write(
            socket_,
            net::buffer(message_),
            [self](beast::error_code ec, std::size_t /*bytes_transferred*/) {
                if (ec) {
                    return ReportError(ec, "write");
                }
                self->Shutdown();
            });
    }

    void Shutdown() {
        beast::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        if (ec && ec != beast::errc::not_connected) {
            ReportError(ec, "shutdown");
        }
    }

private:
    tcp::socket socket_;
    std::string message_ = "client connected\n";
};

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:
    template <typename Handler>
    Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler)
        : ioc_(ioc)
        // Обработчики асинхронных операций acceptor_ будут вызываться в своём strand
        , acceptor_(net::make_strand(ioc))
        , request_handler_(std::forward<Handler>(request_handler)) {
        // Открываем acceptor, используя протокол (IPv4 или IPv6), указанный в endpoint
        acceptor_.open(endpoint.protocol());

        // После закрытия TCP-соединения сокет некоторое время может считаться занятым,
        // чтобы компьютеры могли обменяться завершающими пакетами данных.
        // Однако это может помешать повторно открыть сокет в полузакрытом состоянии.
        // Флаг reuse_address разрешает открыть сокет, когда он "наполовину закрыт"
        acceptor_.set_option(net::socket_base::reuse_address(true));

        // Привязываем acceptor к адресу и порту endpoint
        acceptor_.bind(endpoint);

        // Переводим acceptor в состояние, в котором он способен принимать новые соединения
        // Благодаря этому новые подключения будут помещаться в очередь ожидающих соединений
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void Run() {
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            // Передаём последовательный исполнитель, в котором будут вызываться обработчики
            // асинхронных операций сокета
            net::make_strand(ioc_),

            // С помощью bind_front_handler создаём обработчик, привязанный к методу OnAccept
            // текущего объекта.
            // Так как Listener — шаблонный класс, нужно подсказать компилятору, что
            // shared_from_this — метод класса, а не свободная функция.
            // Для этого вызываем его, используя this.
            beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));
    }

    // Метод acceptor::async_accept создаст сокет и передаст его в OnAccept
    void OnAccept(sys::error_code ec, tcp::socket socket) {
        if (ec) {
            return ReportError(ec, "accept");
        }

        // Асинхронно обрабатываем сессию
        AsyncRunSession(std::move(socket));

        // Принимаем новое соединение
        DoAccept();
    }

    void AsyncRunSession(tcp::socket&& socket) {
        // Пока RequestHandler не используется для реального HTTP,
        // но мы уже храним его в архитектуре, как в теории урока.
        [[maybe_unused]] auto& handler = request_handler_;

        std::make_shared<Session>(std::move(socket))->Run();
    }

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

}  // namespace http_server

int main() {
    using namespace std::literals;

    try {
        const auto num_threads = std::max(1u, std::thread::hardware_concurrency());

        net::io_context ioc(static_cast<int>(num_threads));

        // Подписка на SIGINT и SIGTERM для корректного завершения
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Signal " << signal_number << " received" << std::endl;
                ioc.stop();
            }
        });

        const auto address = net::ip::make_address("127.0.0.1");
        constexpr unsigned short port = 8080;

        // Пока это просто заглушка под будущий RequestHandler.
        // Позже здесь будет полноценный HTTP-обработчик.
        auto request_handler = [](auto&&...) {};

        auto listener = std::make_shared<http_server::Listener<decltype(request_handler)>>(
            ioc,
            http_server::tcp::endpoint{address, port},
            request_handler);

        listener->Run();

        std::cout << "Demo 3: async accept server" << std::endl;
        std::cout << "Listening on http://127.0.0.1:8080" << std::endl;
        std::cout << "Connect with: nc 127.0.0.1 8080" << std::endl;
        std::cout << "Stop with Ctrl+C or kill <PID>" << std::endl;

        RunWorkers(num_threads, [&ioc] {
            ioc.run();
        });

        std::cout << "Server stopped" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
