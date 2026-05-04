#pragma once

#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

#include <cassert>
#include <filesystem>
#include <memory>
#include <utility>
#include <variant>

namespace http_handler {

namespace net = boost::asio;
namespace fs = std::filesystem;

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
public:
    using Strand = net::strand<net::io_context::executor_type>;

    RequestHandler(fs::path static_files_root, Strand api_strand)
        : static_files_root_{std::move(static_files_root)}
        , api_strand_{api_strand} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Request, typename Send>
    void operator()(Request&& request, Send&& send) {
        if (IsApiRequest(request)) {
            auto handle = [
                self = shared_from_this(),
                request = std::forward<Request>(request),
                send = std::forward<Send>(send)
            ]() mutable {
                assert(self->api_strand_.running_in_this_thread());
                send(self->HandleApiRequest(request));
            };

            return net::dispatch(api_strand_, std::move(handle));
        }

        send(HandleFileRequest(request));
    }

private:
    template <typename Request>
    bool IsApiRequest(const Request& request) const {
        return request.target().starts_with("/api/");
    }

    template <typename Request>
    std::string HandleApiRequest(const Request&) {
        return "API response";
    }

    template <typename Request>
    std::string HandleFileRequest(const Request&) {
        return "File response";
    }

    fs::path static_files_root_;
    Strand api_strand_;
};

}  // namespace http_handler
