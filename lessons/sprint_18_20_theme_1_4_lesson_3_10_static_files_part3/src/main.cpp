#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace fs = std::filesystem;

using tcp = net::ip::tcp;

bool IsHexDigit(char c) {
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

int HexToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }

    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }

    return -1;
}

std::string UrlDecode(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (std::size_t i = 0; i < value.size(); ++i) {
        char current = value[i];

        if (current == '+') {
            result.push_back(' ');
            continue;
        }

        if (current == '%' && i + 2 < value.size()) {
            char high = value[i + 1];
            char low = value[i + 2];

            if (IsHexDigit(high) && IsHexDigit(low)) {
                int decoded = HexToInt(high) * 16 + HexToInt(low);
                result.push_back(static_cast<char>(decoded));
                i += 2;
                continue;
            }
        }

        result.push_back(current);
    }

    return result;
}

bool StartsWith(const std::string& value, const std::string& prefix) {
    return value.size() >= prefix.size()
        && value.compare(0, prefix.size(), prefix) == 0;
}

std::string GetMimeType(const fs::path& path) {
    static const std::unordered_map<std::string, std::string> types = {
        {".html", "text/html; charset=UTF-8"},
        {".htm", "text/html; charset=UTF-8"},
        {".css", "text/css; charset=UTF-8"},
        {".js", "application/javascript; charset=UTF-8"},
        {".json", "application/json; charset=UTF-8"},

        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".ico", "image/vnd.microsoft.icon"},

        {".mp3", "audio/mpeg"},
        {".wav", "audio/wav"},
        {".ogg", "audio/ogg"},

        {".mp4", "video/mp4"},
        {".webm", "video/webm"},
        {".ogv", "video/ogg"},

        {".txt", "text/plain; charset=UTF-8"}
    };

    auto extension = path.extension().string();
    auto it = types.find(extension);

    if (it != types.end()) {
        return it->second;
    }

    return "application/octet-stream";
}

std::string ReadFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open file");
    }

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

http::response<http::string_body> MakeTextResponse(
    http::status status,
    std::string text,
    unsigned version
) {
    http::response<http::string_body> response(status, version);
    response.set(http::field::content_type, "text/plain; charset=UTF-8");
    response.body() = std::move(text);
    response.prepare_payload();
    return response;
}

http::response<http::string_body> HandleApiRequest(
    const http::request<http::string_body>& request
) {
    std::string target = std::string(request.target());

    if (target == "/api/v1/maps") {
        http::response<http::string_body> response(http::status::ok, request.version());
        response.set(http::field::content_type, "application/json; charset=UTF-8");
        response.body() =
            "["
            "{\"id\":\"map1\",\"name\":\"Training Map\"},"
            "{\"id\":\"map2\",\"name\":\"Media Test Map\"}"
            "]";
        response.prepare_payload();
        return response;
    }

    if (target == "/api/v1/maps/map1") {
        http::response<http::string_body> response(http::status::ok, request.version());
        response.set(http::field::content_type, "application/json; charset=UTF-8");
        response.body() =
            "{"
            "\"id\":\"map1\","
            "\"name\":\"Training Map\","
            "\"roads\":[\"road_vh\",\"road_tr\",\"road_t\"]"
            "}";
        response.prepare_payload();
        return response;
    }

    if (target == "/api/v1/maps/map2") {
        http::response<http::string_body> response(http::status::ok, request.version());
        response.set(http::field::content_type, "application/json; charset=UTF-8");
        response.body() =
            "{"
            "\"id\":\"map2\","
            "\"name\":\"Media Test Map\","
            "\"resources\":[\"image\",\"audio\",\"video\"]"
            "}";
        response.prepare_payload();
        return response;
    }

    return MakeTextResponse(
        http::status::not_found,
        "404 API resource not found",
        request.version()
    );
}

http::response<http::string_body> HandleStaticFileRequest(
    const http::request<http::string_body>& request,
    const fs::path& static_root
) {
    if (request.method() != http::verb::get) {
        return MakeTextResponse(
            http::status::method_not_allowed,
            "Only GET is allowed",
            request.version()
        );
    }

    std::string encoded_target = std::string(request.target());

    std::string target = UrlDecode(encoded_target);

    fs::path requested_path;

    if (target == "/") {
        requested_path = static_root / "index.html";
    } else {
        requested_path = static_root / target.substr(1);
    }

    if (!IsSubPath(requested_path, static_root)) {
        return MakeTextResponse(
            http::status::forbidden,
            "403 Forbidden",
            request.version()
        );
    }

    if (!fs::exists(requested_path) || !fs::is_regular_file(requested_path)) {
        return MakeTextResponse(
            http::status::not_found,
            "404 Not Found",
            request.version()
        );
    }

    std::string body = ReadFile(requested_path);

    http::response<http::string_body> response(http::status::ok, request.version());
    response.set(http::field::content_type, GetMimeType(requested_path));
    response.body() = std::move(body);
    response.prepare_payload();

    return response;
}

http::response<http::string_body> HandleRequest(
    const http::request<http::string_body>& request,
    const fs::path& static_root
) {
    std::string target = std::string(request.target());

    if (StartsWith(target, "/api/")) {
        return HandleApiRequest(request);
    }

    return HandleStaticFileRequest(request, static_root);
}

void HandleSession(tcp::socket socket, const fs::path& static_root) {
    beast::flat_buffer buffer;

    http::request<http::string_body> request;
    http::read(socket, buffer, request);

    std::cout << request.method_string() << " " << request.target() << "\n";

    auto response = HandleRequest(request, static_root);

    http::write(socket, response);

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    try {
        const auto address = net::ip::make_address("0.0.0.0");
        const unsigned short port = 8080;

        fs::path static_root = fs::current_path().parent_path() / "static";

        net::io_context io_context;

        tcp::acceptor acceptor(io_context, {address, port});

        std::cout << "Server started: http://localhost:8080\n";
        std::cout << "Static root: " << static_root << "\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            HandleSession(std::move(socket), static_root);
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
