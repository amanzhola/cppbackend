#include "request_handler.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>

namespace http_handler {

namespace fs = std::filesystem;

namespace {

std::string GetMimeType(const fs::path& path) {
    static const std::unordered_map<std::string, std::string> types = {
        {".html", "text/html"},
        {".htm", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".json", "application/json"},

        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".ico", "image/vnd.microsoft.icon"},
        {".svg", "image/svg+xml"},
        {".svgz", "image/svg+xml"},

        {".mp3", "audio/mpeg"},

        {".txt", "text/plain"},
        {".fbx", "application/octet-stream"}
    };

    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    if (const auto it = types.find(ext); it != types.end()) {
        return it->second;
    }

    return "application/octet-stream";
}

std::string ReadFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

bool IsSubPath(const fs::path& path, const fs::path& base) {
    auto canonical_path = fs::weakly_canonical(path);
    auto canonical_base = fs::weakly_canonical(base);

    auto path_it = canonical_path.begin();
    auto base_it = canonical_base.begin();

    for (; base_it != canonical_base.end(); ++base_it, ++path_it) {
        if (path_it == canonical_path.end() || *path_it != *base_it) {
            return false;
        }
    }

    return true;
}

}  // namespace

RequestHandler::RequestHandler(model::Game& game, std::filesystem::path static_root)
    : game_{game}
    , static_root_{std::move(static_root)} {
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

RequestHandler::StringResponse RequestHandler::HandleStaticFile(
    const http::request<http::string_body>& req,
    unsigned version,
    bool keep_alive) const {
    std::string target = std::string(req.target());

    fs::path requested_path;

    if (target == "/") {
        requested_path = static_root_ / "index.html";
    } else {
        requested_path = static_root_ / target.substr(1);
    }

    if (!IsSubPath(requested_path, static_root_)) {
        return MakeBadRequest(version, keep_alive, kBadRequestCode, kBadRequestMessage);
    }

    if (fs::is_directory(requested_path)) {
        requested_path /= "index.html";
    }

    if (!fs::exists(requested_path) || !fs::is_regular_file(requested_path)) {
        StringResponse response(http::status::not_found, version);
        response.set(http::field::content_type, "text/plain");
        response.body() = "File not found";
        response.content_length(response.body().size());
        response.keep_alive(keep_alive);
        return response;
    }

    std::string body = ReadFile(requested_path);

    StringResponse response(http::status::ok, version);
    response.set(http::field::content_type, GetMimeType(requested_path));
    response.body() = std::move(body);
    response.content_length(response.body().size());
    response.keep_alive(keep_alive);

    return response;
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

}  // namespace http_handler
