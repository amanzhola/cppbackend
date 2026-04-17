#pragma once

// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

namespace http_server {

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;
using namespace std::literals;

void ReportError(beast::error_code ec, std::string_view what) {
    std::cerr << what << ": "sv << ec.message() << std::endl;
}

class SessionBase {
public:
    using HttpRequest = http::request<http::string_body>;

    // Запрещаем копирование и присваивание объектов SessionBase и его наследников
    SessionBase(const SessionBase&) = delete;
    SessionBase& operator=(const SessionBase&) = delete;

    void Run() {
        // Вызываем Read, используя executor stream_.
        // Так вся дальнейшая работа со stream_ пойдёт через его executor.
        net::dispatch(stream_.get_executor(),
                      beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

protected:
    explicit SessionBase(tcp::socket&& socket)
        : stream_(std::move(socket)) {
    }

    template <typename Body, typename Fields>
    void Write(http::response<Body, Fields>&& response) {
        // Запись асинхронная, поэтому response должен жить до конца async_write.
        auto safe_response =
            std::make_shared<http::response<Body, Fields>>(std::move(response));

        auto self = GetSharedThis();

        http::async_write(
            stream_,
            *safe_response,
            [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                self->OnWrite(safe_response->need_eof(), ec, bytes_written);
            });
    }

    ~SessionBase() = default;

private:
    void Read() {
        // Метод может вызываться много раз в рамках keep-alive,
        // поэтому старый запрос нужно очистить.
        request_ = {};

        // Ограничиваем время чтения запроса
        stream_.expires_after(std::chrono::seconds(30));

        // Асинхронно читаем HTTP-запрос
        http::async_read(
            stream_,
            buffer_,
            request_,
            beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
    }

    void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
        if (ec == http::error::end_of_stream) {
            // Нормальная ситуация: клиент закрыл соединение
            return Close();
        }

        if (ec) {
            return ReportError(ec, "read"sv);
        }

        HandleRequest(std::move(request_));
    }

    void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
        if (ec) {
            return ReportError(ec, "write"sv);
        }

        if (close) {
            // Семантика ответа требует закрыть соединение
            return Close();
        }

        // Продолжаем читать следующий запрос
        Read();
    }

    void Close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

    virtual void HandleRequest(HttpRequest&& request) = 0;
    virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;

private:
    // tcp_stream содержит внутри себя socket и добавляет поддержку таймаутов
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    HttpRequest request_;
};

template <typename RequestHandler>
class Session
    : public SessionBase
    , public std::enable_shared_from_this<Session<RequestHandler>> {
public:
    template <typename Handler>
    Session(tcp::socket&& socket, Handler&& request_handler)
        : SessionBase(std::move(socket))
        , request_handler_(std::forward<Handler>(request_handler)) {
    }

private:
    void HandleRequest(HttpRequest&& request) override {
        // request_handler_ принимает:
        // 1. запрос
        // 2. sender-функцию, которая отправляет ответ клиенту
        request_handler_(std::move(request),
                         [self = this->shared_from_this()](auto&& response) {
                             self->Write(std::move(response));
                         });
    }

    std::shared_ptr<SessionBase> GetSharedThis() override {
        return this->shared_from_this();
    }

private:
    RequestHandler request_handler_;
};

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:
    template <typename Handler>
    Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler)
        : ioc_(ioc)
        // acceptor будет вызывать свои обработчики в своём strand
        , acceptor_(net::make_strand(ioc))
        , request_handler_(std::forward<Handler>(request_handler)) {
        // Открываем acceptor с нужным протоколом (IPv4 / IPv6)
        acceptor_.open(endpoint.protocol());

        // Разрешаем повторно использовать адрес
        acceptor_.set_option(net::socket_base::reuse_address(true));

        // Привязываем acceptor к адресу и порту
        acceptor_.bind(endpoint);

        // Переводим acceptor в режим ожидания входящих соединений
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void Run() {
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            // У нового socket будет свой strand
            net::make_strand(ioc_),
            beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));
    }

    void OnAccept(sys::error_code ec, tcp::socket socket) {
        if (ec) {
            return ReportError(ec, "accept"sv);
        }

        AsyncRunSession(std::move(socket));

        // Сразу ждём следующее подключение
        DoAccept();
    }

    void AsyncRunSession(tcp::socket&& socket) {
        std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();
    }

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

template <typename RequestHandler>
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler) {
    using MyListener = Listener<std::decay_t<RequestHandler>>;

    std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
}

}  // namespace http_server
