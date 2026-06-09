#pragma once

#include "loot_generator.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tagged.h"
#include "collision_detector.h"
#include <deque>

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

std::string DirectionToString(Direction direction);

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

    Road(HorizontalTag, Point start, Coord end_x) noexcept;
    Road(VerticalTag, Point start, Coord end_y) noexcept;

    bool IsHorizontal() const noexcept;
    bool IsVertical() const noexcept;

    Point GetStart() const noexcept;
    Point GetEnd() const noexcept;

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept;

    const Rectangle& GetBounds() const noexcept;

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept;

    const Id& GetId() const noexcept;
    Point GetPosition() const noexcept;
    Offset GetOffset() const noexcept;

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

    Map(Id id, std::string name) noexcept;

    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;

    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;

    double GetDogSpeed() const noexcept;
    void SetDogSpeed(double dog_speed) noexcept;

    size_t GetLootTypesCount() const noexcept;
    void SetLootTypesCount(size_t loot_types_count) noexcept;

    size_t GetBagCapacity() const noexcept {
        return bag_capacity_;
    }

    void SetBagCapacity(size_t capacity) noexcept {
        bag_capacity_ = capacity;
    }

    const std::vector<int>& GetLootTypeValues() const noexcept {
        return loot_type_values_;
    }

    void SetLootTypeValues(std::vector<int> values) {
        loot_type_values_ = std::move(values);
    }

    void AddRoad(const Road& road);
    void AddBuilding(const Building& building);
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
    size_t loot_types_count_ = 0;
    size_t bag_capacity_ = 3;
    std::vector<int> loot_type_values_;
};

struct LostObject {
    using Id = std::uint32_t;

    Id id = 0;
    std::uint32_t type = 0;
    Position position;
};

class Dog {
public:
    using Id = std::uint32_t;

    struct CollectedObject {
        LostObject::Id id;
        std::uint32_t type;
    };

    using Bag = std::vector<CollectedObject>;

    Dog(Id id, std::string name, Position position);

    Id GetId() const noexcept;
    const std::string& GetName() const noexcept;

    Position GetPosition() const noexcept;
    Speed GetSpeed() const noexcept;
    Direction GetDirection() const noexcept;

    const Bag& GetBag() const noexcept {
        return bag_;
    }

    bool IsBagFull(size_t capacity) const noexcept {
        return bag_.size() >= capacity;
    }

    void AddToBag(LostObject object) {
        bag_.push_back({object.id, object.type});
    }

    void ClearBag() noexcept {
        bag_.clear();
    }

    std::vector<CollectedObject> DropBag() {
        std::vector<CollectedObject> result;
        result.swap(bag_);
        return result;
    }

    int GetScore() const noexcept {
        return score_;
    }

    void AddScore(int score) noexcept {
        score_ += score;
    }

    void SetPosition(Position position) noexcept;
    void SetSpeed(Speed speed) noexcept;
    void SetDirection(Direction direction) noexcept;
    void Stop() noexcept;

    void AddGameTime(std::chrono::milliseconds delta) noexcept;

	std::chrono::milliseconds GetPlayTime() const noexcept {
    	return play_time_;
    }

    std::chrono::milliseconds GetIdleTime() const noexcept {
    	return idle_time_;
    }

private:
    Id id_;
    std::string name_;
    Position position_;
    Speed speed_{0.0, 0.0};
    Direction direction_ = Direction::NORTH;

    Bag bag_;
    int score_ = 0;

    std::chrono::milliseconds play_time_{0};
    std::chrono::milliseconds idle_time_{0};
};

class GameSession {
public:
    GameSession(const Map& map, bool randomize_spawn_points);

    Dog& AddDog(std::string name);
    Dog& AddRestoredDog(Dog dog);

    Dog* FindDog(Dog::Id id);

    const Map& GetMap() const noexcept;
    const std::unordered_map<Dog::Id, Dog>& GetDogs() const noexcept;
    const std::unordered_map<LostObject::Id, LostObject>& GetLostObjects() const noexcept;

    void AddRestoredLostObject(LostObject object);

    void SetNextDogId(Dog::Id id) noexcept;
    void SetNextLostObjectId(LostObject::Id id) noexcept;

    Dog::Id GetNextDogId() const noexcept;
    LostObject::Id GetNextLostObjectId() const noexcept;

    void Update(double delta_seconds);

    void GenerateLoot(std::chrono::milliseconds time_delta,
                      loot_gen::LootGenerator& loot_generator);

    void ReturnDogLootToOffice(Dog& dog);

    void RemoveDog(Dog::Id id);

private:
    static constexpr double ROAD_HALF_WIDTH = 0.4;

    Position GenerateStartPosition() const;
    Position GenerateRandomPosition() const;
    Position GenerateRandomRoadPosition() const;
    LostObject GenerateLostObject();

    void MoveDog(Dog& dog, double delta_seconds) const;
    Position LimitPositionToRoads(Position current, Position target) const;

    const Map& map_;
    bool randomize_spawn_points_ = false;
    Dog::Id next_dog_id_ = 0;
    std::unordered_map<Dog::Id, Dog> dogs_;

    LostObject::Id next_lost_object_id_ = 0;
    std::unordered_map<LostObject::Id, LostObject> lost_objects_;
};

class Game {
public:
    using Maps = std::vector<Map>;
    using Sessions = std::deque<GameSession>;

    void AddMap(Map map);

    const Maps& GetMaps() const noexcept;
    const Sessions& GetSessions() const noexcept;
    const Map* FindMap(const Map::Id& id) const noexcept;

    void SetRandomizeSpawnPoints(bool value) noexcept;
    void SetLootGeneratorConfig(std::chrono::milliseconds period, double probability);

    GameSession& FindOrCreateSession(const Map& map);


    void Update(double delta_seconds);

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
    Sessions sessions_;
    bool randomize_spawn_points_ = false;

    loot_gen::LootGenerator loot_generator_{std::chrono::milliseconds{1000}, 0.0};
};

}  // namespace model
