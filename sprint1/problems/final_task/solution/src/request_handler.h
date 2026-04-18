#pragma once
#include "http_server.h"
#include "model.h"

#include <boost/json.hpp>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        const auto target = std::string(req.target());

        if (req.method() != http::verb::get) {
            return send(MakeBadRequest(req, "badRequest", "Bad request"));
        }

        if (target == "/api/v1/maps") {
            return send(MakeJsonResponse(req, SerializeMaps()));
        }

        constexpr std::string_view maps_prefix = "/api/v1/maps/"sv;
        if (target.starts_with(maps_prefix)) {
            const std::string map_id = target.substr(maps_prefix.size());

            if (map_id.empty()) {
                return send(MakeBadRequest(req, "badRequest", "Bad request"));
            }

            if (const model::Map* map = game_.FindMap(model::Map::Id(map_id))) {
                return send(MakeJsonResponse(req, SerializeMap(*map)));
            }

            return send(MakeNotFound(req, "mapNotFound", "Map not found"));
        }

        if (target.starts_with("/api/"sv)) {
            return send(MakeBadRequest(req, "badRequest", "Bad request"));
        }

        return send(MakeBadRequest(req, "badRequest", "Bad request"));
    }

private:
    using StringResponse = http::response<http::string_body>;

    StringResponse MakeJsonResponse(const auto& req, json::value body, http::status status = http::status::ok) const {
        StringResponse response(status, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = json::serialize(body);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    }

    StringResponse MakeErrorResponse(const auto& req,
                                     http::status status,
                                     std::string_view code,
                                     std::string_view message) const {
        json::object obj;
        obj["code"] = std::string(code);
        obj["message"] = std::string(message);
        return MakeJsonResponse(req, obj, status);
    }

    StringResponse MakeBadRequest(const auto& req,
                                  std::string_view code,
                                  std::string_view message) const {
        return MakeErrorResponse(req, http::status::bad_request, code, message);
    }

    StringResponse MakeNotFound(const auto& req,
                                std::string_view code,
                                std::string_view message) const {
        return MakeErrorResponse(req, http::status::not_found, code, message);
    }

    json::value SerializeMaps() const {
        json::array maps;

        for (const auto& map : game_.GetMaps()) {
            json::object obj;
            obj["id"] = *map.GetId();
            obj["name"] = map.GetName();
            maps.emplace_back(std::move(obj));
        }

        return maps;
    }

    json::value SerializeMap(const model::Map& map) const {
        json::object obj;
        obj["id"] = *map.GetId();
        obj["name"] = map.GetName();

        json::array roads;
        for (const auto& road : map.GetRoads()) {
            json::object road_obj;
            road_obj["x0"] = road.GetStart().x;
            road_obj["y0"] = road.GetStart().y;

            if (road.IsHorizontal()) {
                road_obj["x1"] = road.GetEnd().x;
            } else {
                road_obj["y1"] = road.GetEnd().y;
            }

            roads.emplace_back(std::move(road_obj));
        }
        obj["roads"] = std::move(roads);

        json::array buildings;
        for (const auto& building : map.GetBuildings()) {
            json::object building_obj;
            const auto& bounds = building.GetBounds();
            building_obj["x"] = bounds.position.x;
            building_obj["y"] = bounds.position.y;
            building_obj["w"] = bounds.size.width;
            building_obj["h"] = bounds.size.height;
            buildings.emplace_back(std::move(building_obj));
        }
        obj["buildings"] = std::move(buildings);

        json::array offices;
        for (const auto& office : map.GetOffices()) {
            json::object office_obj;
            office_obj["id"] = *office.GetId();
            office_obj["x"] = office.GetPosition().x;
            office_obj["y"] = office.GetPosition().y;
            office_obj["offsetX"] = office.GetOffset().dx;
            office_obj["offsetY"] = office.GetOffset().dy;
            offices.emplace_back(std::move(office_obj));
        }
        obj["offices"] = std::move(offices);

        return obj;
    }

private:
    model::Game& game_;
};

}  // namespace http_handler
