#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <chrono>
#include <functional>
#include <memory>

#include "hotdog.h"
#include "result.h"

namespace net = boost::asio;

// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;

// Класс "Кафетерий". Готовит хот-доги
class Cafeteria {
public:
    explicit Cafeteria(net::io_context& io)
        : io_{io}
        , strand_{net::make_strand(io_)} {
    }

    // Асинхронно готовит хот-дог и вызывает handler, как только хот-дог будет готов.
    // Этот метод может быть вызван из произвольного потока
    void OrderHotDog(HotDogHandler handler) {
        net::dispatch(strand_, [this, handler = std::move(handler)]() mutable {
            auto bread = store_.GetBread();
            auto sausage = store_.GetSausage();
            auto hotdog_id = ++next_hotdog_id_;

            auto order = std::make_shared<OrderState>(
                io_,
                gas_cooker_,
                std::move(bread),
                std::move(sausage),
                hotdog_id,
                std::move(handler));

            try {
                order->Start();
            } catch (...) {
                order->Complete(Result<HotDog>::FromCurrentException());
            }
        });
    }

private:
    using Strand = net::strand<net::io_context::executor_type>;

    struct OrderState : std::enable_shared_from_this<OrderState> {
        net::io_context& io;
        Strand strand;
        std::shared_ptr<GasCooker> gas_cooker;
        std::shared_ptr<Bread> bread;
        std::shared_ptr<Sausage> sausage;
        int hotdog_id;
        HotDogHandler handler;
        net::steady_timer bread_timer;
        net::steady_timer sausage_timer;
        bool bread_ready = false;
        bool sausage_ready = false;
        bool completed = false;

        OrderState(net::io_context& io_,
                   std::shared_ptr<GasCooker> gas_cooker_,
                   std::shared_ptr<Bread> bread_,
                   std::shared_ptr<Sausage> sausage_,
                   int hotdog_id_,
                   HotDogHandler handler_)
            : io{io_}
            , strand{net::make_strand(io)}
            , gas_cooker{std::move(gas_cooker_)}
            , bread{std::move(bread_)}
            , sausage{std::move(sausage_)}
            , hotdog_id{hotdog_id_}
            , handler{std::move(handler_)}
            , bread_timer{io}
            , sausage_timer{io} {
        }

        void Start() {
            auto self = this->shared_from_this();

            bread->StartBake(*gas_cooker, [self] {
                net::dispatch(self->strand, [self] {
                    self->bread_timer.expires_after(std::chrono::seconds{1});
                    self->bread_timer.async_wait(net::bind_executor(
                        self->strand,
                        [self](const boost::system::error_code& ec) {
                            if (self->completed) {
                                return;
                            }
                            if (ec) {
                                self->Complete(Result<HotDog>{
                                    std::make_exception_ptr(std::system_error(ec))});
                                return;
                            }

                            try {
                                self->bread->StopBaking();
                                self->bread_ready = true;
                                self->TryAssemble();
                            } catch (...) {
                                self->Complete(Result<HotDog>::FromCurrentException());
                            }
                        }));
                });
            });

            sausage->StartFry(*gas_cooker, [self] {
                net::dispatch(self->strand, [self] {
                    self->sausage_timer.expires_after(std::chrono::milliseconds{1500});
                    self->sausage_timer.async_wait(net::bind_executor(
                        self->strand,
                        [self](const boost::system::error_code& ec) {
                            if (self->completed) {
                                return;
                            }
                            if (ec) {
                                self->Complete(Result<HotDog>{
                                    std::make_exception_ptr(std::system_error(ec))});
                                return;
                            }

                            try {
                                self->sausage->StopFry();
                                self->sausage_ready = true;
                                self->TryAssemble();
                            } catch (...) {
                                self->Complete(Result<HotDog>::FromCurrentException());
                            }
                        }));
                });
            });
        }

        void TryAssemble() {
            if (completed) {
                return;
            }

            if (!bread_ready || !sausage_ready) {
                return;
            }

            try {
                Complete(Result<HotDog>{HotDog{hotdog_id, sausage, bread}});
            } catch (...) {
                Complete(Result<HotDog>::FromCurrentException());
            }
        }

        void Complete(Result<HotDog> result) {
            if (completed) {
                return;
            }

            completed = true;
            auto h = std::move(handler);

            net::post(io, [h = std::move(h), result = std::move(result)]() mutable {
                h(std::move(result));
            });
        }
    };

    net::io_context& io_;
    Strand strand_;
    // Используется для создания ингредиентов хот-дога
    Store store_;
    int next_hotdog_id_ = 0;

    // Газовая плита. По условию задачи в кафетерии есть только одна газовая плита на 8 горелок
    // Используйте её для приготовления ингредиентов хот-дога.
    // Плита создаётся с помощью make_shared, так как GasCooker унаследован от
    // enable_shared_from_this.
    std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);
};
