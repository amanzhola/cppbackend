#pragma once

#include "dog.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace model {

class Map;

class GameSession {
public:
    using Id = std::uint64_t;

    explicit GameSession(const Map& map)
        : map_{map} {
    }

    Dog& AddDog(std::string name) {
        const Dog::Id dog_id = next_dog_id_++;
        auto [it, inserted] = dogs_.emplace(dog_id, Dog{dog_id, std::move(name)});
        return it->second;
    }

    const std::unordered_map<Dog::Id, Dog>& GetDogs() const noexcept {
        return dogs_;
    }

    const Map& GetMap() const noexcept {
        return map_;
    }

private:
    const Map& map_;
    Dog::Id next_dog_id_ = 1;
    std::unordered_map<Dog::Id, Dog> dogs_;
};

}  // namespace model#pragma once

#include "dog.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace model {

class Map;

class GameSession {
public:
    using Id = std::uint64_t;

    explicit GameSession(const Map& map)
        : map_{map} {
    }

    Dog& AddDog(std::string name) {
        const Dog::Id dog_id = next_dog_id_++;
        auto [it, inserted] = dogs_.emplace(dog_id, Dog{dog_id, std::move(name)});
        return it->second;
    }

    const std::unordered_map<Dog::Id, Dog>& GetDogs() const noexcept {
        return dogs_;
    }

    const Map& GetMap() const noexcept {
        return map_;
    }

private:
    const Map& map_;
    Dog::Id next_dog_id_ = 1;
    std::unordered_map<Dog::Id, Dog> dogs_;
};

}  // namespace model
