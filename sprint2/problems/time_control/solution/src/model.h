#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tagged.h"

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

struct Position {
    double x = 0.0;
    double y = 0.0;
};

struct Speed {
    double dx = 0.0;
    double dy = 0.0;
};

enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST
};

inline std::string DirectionToString(Direction direction) {
    switch (direction) {
        case Direction::NORTH:
            return "U";
        case Direction::SOUTH:
            return "D";
        case Direction::WEST:
            return "L";
        case Direction::EAST:
            return "R";
    }

    return "U";
}

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    double GetDogSpeed() const noexcept {
        return dog_speed_;
    }

    void SetDogSpeed(double dog_speed) noexcept {
        dog_speed_ = dog_speed;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;

    double dog_speed_ = 1.0;
};

class Dog {
public:
    using Id = std::uint32_t;

    Dog(Id id, std::string name, Position position)
        : id_{id}
        , name_{std::move(name)}
        , position_{position} {
    }

    Id GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    Position GetPosition() const noexcept {
        return position_;
    }

    Speed GetSpeed() const noexcept {
        return speed_;
    }

    Direction GetDirection() const noexcept {
        return direction_;
    }

    void SetPosition(Position position) noexcept {
        position_ = position;
    }

    void SetSpeed(Speed speed) noexcept {
        speed_ = speed;
    }

    void SetDirection(Direction direction) noexcept {
        direction_ = direction;
    }

    void Stop() noexcept {
        speed_ = {0.0, 0.0};
    }

private:
    Id id_;
    std::string name_;
    Position position_;
    Speed speed_{0.0, 0.0};
    Direction direction_ = Direction::NORTH;
};

class GameSession {
public:
    explicit GameSession(const Map& map)
        : map_{map} {
    }

    Dog& AddDog(std::string name) {
        Dog::Id id = next_dog_id_++;
        Position position = GenerateStartPosition();
        auto [it, inserted] = dogs_.emplace(id, Dog{id, std::move(name), position});
        return it->second;
    }

    Dog* FindDog(Dog::Id id) {
        if (auto it = dogs_.find(id); it != dogs_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    const Map& GetMap() const noexcept {
        return map_;
    }

    const std::unordered_map<Dog::Id, Dog>& GetDogs() const noexcept {
        return dogs_;
    }

    void Update(double delta_seconds) {
        for (auto& [dog_id, dog] : dogs_) {
            MoveDog(dog, delta_seconds);
        }
    }

private:
    static constexpr double ROAD_HALF_WIDTH = 0.4;

    Position GenerateStartPosition() const {
        const auto& roads = map_.GetRoads();

        if (roads.empty()) {
            return {0.0, 0.0};
        }

        const Point start = roads.front().GetStart();
        return {static_cast<double>(start.x), static_cast<double>(start.y)};
    }

    void MoveDog(Dog& dog, double delta_seconds) const {
        const Position current = dog.GetPosition();
        const Speed speed = dog.GetSpeed();

        if (speed.dx == 0.0 && speed.dy == 0.0) {
            return;
        }

        Position target{
            current.x + speed.dx * delta_seconds,
            current.y + speed.dy * delta_seconds
        };

        Position limited = LimitPositionToRoads(current, target);

        dog.SetPosition(limited);

        if (limited.x != target.x || limited.y != target.y) {
            dog.Stop();
        }
    }

    Position LimitPositionToRoads(Position current, Position target) const {
        Position result = target;

        bool found_road = false;

        double min_x = current.x;
        double max_x = current.x;
        double min_y = current.y;
        double max_y = current.y;

        for (const Road& road : map_.GetRoads()) {
            const Point start = road.GetStart();
            const Point end = road.GetEnd();

            if (road.IsHorizontal()) {
                const double road_y = static_cast<double>(start.y);
                const double road_min_x = static_cast<double>(std::min(start.x, end.x)) - ROAD_HALF_WIDTH;
                const double road_max_x = static_cast<double>(std::max(start.x, end.x)) + ROAD_HALF_WIDTH;
                const double road_min_y = road_y - ROAD_HALF_WIDTH;
                const double road_max_y = road_y + ROAD_HALF_WIDTH;

                if (current.x >= road_min_x && current.x <= road_max_x
                    && current.y >= road_min_y && current.y <= road_max_y) {
                    found_road = true;
                    min_x = std::min(min_x, road_min_x);
                    max_x = std::max(max_x, road_max_x);
                    min_y = std::min(min_y, road_min_y);
                    max_y = std::max(max_y, road_max_y);
                }
            } else if (road.IsVertical()) {
                const double road_x = static_cast<double>(start.x);
                const double road_min_x = road_x - ROAD_HALF_WIDTH;
                const double road_max_x = road_x + ROAD_HALF_WIDTH;
                const double road_min_y = static_cast<double>(std::min(start.y, end.y)) - ROAD_HALF_WIDTH;
                const double road_max_y = static_cast<double>(std::max(start.y, end.y)) + ROAD_HALF_WIDTH;

                if (current.x >= road_min_x && current.x <= road_max_x
                    && current.y >= road_min_y && current.y <= road_max_y) {
                    found_road = true;
                    min_x = std::min(min_x, road_min_x);
                    max_x = std::max(max_x, road_max_x);
                    min_y = std::min(min_y, road_min_y);
                    max_y = std::max(max_y, road_max_y);
                }
            }
        }

        if (!found_road) {
            return current;
        }

        result.x = std::clamp(result.x, min_x, max_x);
        result.y = std::clamp(result.y, min_y, max_y);

        return result;
    }

    const Map& map_;
    Dog::Id next_dog_id_ = 0;
    std::unordered_map<Dog::Id, Dog> dogs_;
};

class Game {
public:
    using Maps = std::vector<Map>;

    void AddMap(Map map);

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

    GameSession& FindOrCreateSession(const Map& map) {
        for (auto& session : sessions_) {
            if (&session.GetMap() == &map) {
                return session;
            }
        }

        sessions_.emplace_back(map);
        return sessions_.back();
    }

    void Update(double delta_seconds) {
        for (auto& session : sessions_) {
            session.Update(delta_seconds);
        }
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
    std::vector<GameSession> sessions_;
};

}  // namespace model
