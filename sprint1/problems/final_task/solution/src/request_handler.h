#pragma once

#include "http_server.h"
#include "model.h"

#include <boost/json.hpp>

#include <string_view>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game);

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send);

private:
    using StringResponse = http::response<http::string_body>;

    StringResponse MakeJsonResponse(unsigned version,
                                    bool keep_alive,
                                    json::value body,
                                    http::status status = http::status::ok) const;

    StringResponse MakeErrorResponse(unsigned version,
                                     bool keep_alive,
                                     http::status status,
                                     std::string_view code,
                                     std::string_view message) const;

    StringResponse MakeBadRequest(unsigned version,
                                  bool keep_alive,
                                  std::string_view code,
                                  std::string_view message) const;

    StringResponse MakeNotFound(unsigned version,
                                bool keep_alive,
                                std::string_view code,
                                std::string_view message) const;

    json::value SerializeMaps() const;
    json::value SerializeMap(const model::Map& map) const;

private:
    model::Game& game_;
};

}  // namespace http_handler
