#include "request_handler.h"

#include <string>

namespace http_handler {

using namespace std::literals;

namespace {

constexpr std::string_view kApiPrefix = "/api/"sv;
constexpr std::string_view kMapsEndpoint = "/api/v1/maps"sv;
constexpr std::string_view kMapsEndpointPrefix = "/api/v1/maps/"sv;

constexpr std::string_view kBadRequestCode = "badRequest"sv;
constexpr std::string_view kBadRequestMessage = "Bad request"sv;
constexpr std::string_view kMapNotFoundCode = "mapNotFound"sv;
constexpr std::string_view kMapNotFoundMessage = "Map not found"sv;

}  // namespace

RequestHandler::RequestHandler(model::Game& game)
    : game_{game} {
}

RequestHandler::StringResponse RequestHandler::MakeJsonResponse(
    unsigned version,
    bool keep_alive,
    json::value body,
    http::status status) const {
    StringResponse response(status, version);
    response.set(http::field::content_type, "application/json");
    response.body() = json::serialize(body);
    response.content_length(response.body().size());
    response.keep_alive(keep_alive);
    return response;
}

RequestHandler::StringResponse RequestHandler::MakeErrorResponse(
    unsigned version,
    bool keep_alive,
    http::status status,
    std::string_view code,
    std::string_view message) const {
    json::object obj;
    obj["code"] = std::string(code);
    obj["message"] = std::string(message);
    return MakeJsonResponse(version, keep_alive, obj, status);
}

RequestHandler::StringResponse RequestHandler::MakeBadRequest(
    unsigned version,
    bool keep_alive,
    std::string_view code,
    std::string_view message) const {
    return MakeErrorResponse(version, keep_alive, http::status::bad_request, code, message);
}

RequestHandler::StringResponse RequestHandler::MakeNotFound(
    unsigned version,
    bool keep_alive,
    std::string_view code,
    std::string_view message) const {
    return MakeErrorResponse(version, keep_alive, http::status::not_found, code, message);
}

json::value RequestHandler::SerializeMaps() const {
    json::array maps;

    for (const auto& map : game_.GetMaps()) {
        json::object obj;
        obj["id"] = *map.GetId();
        obj["name"] = map.GetName();
        maps.emplace_back(std::move(obj));
    }

    return maps;
}

json::value RequestHandler::SerializeMap(const model::Map& map) const {
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

template <typename Body, typename Allocator, typename Send>
void RequestHandler::operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    const std::string target = std::string(req.target());
    const unsigned version = req.version();
    const bool keep_alive = req.keep_alive();

    if (req.method() != http::verb::get) {
        return send(MakeBadRequest(version, keep_alive, kBadRequestCode, kBadRequestMessage));
    }

    if (target == kMapsEndpoint) {
        return send(MakeJsonResponse(version, keep_alive, SerializeMaps()));
    }

    if (target.starts_with(kMapsEndpointPrefix)) {
        const std::string map_id = target.substr(kMapsEndpointPrefix.size());

        if (map_id.empty()) {
            return send(MakeBadRequest(version, keep_alive, kBadRequestCode, kBadRequestMessage));
        }

        if (const model::Map* map = game_.FindMap(model::Map::Id(map_id))) {
            return send(MakeJsonResponse(version, keep_alive, SerializeMap(*map)));
        }

        return send(MakeNotFound(version, keep_alive, kMapNotFoundCode, kMapNotFoundMessage));
    }

    if (target.starts_with(kApiPrefix)) {
        return send(MakeBadRequest(version, keep_alive, kBadRequestCode, kBadRequestMessage));
    }

    return send(MakeBadRequest(version, keep_alive, kBadRequestCode, kBadRequestMessage));
}

}  // namespace http_handler
