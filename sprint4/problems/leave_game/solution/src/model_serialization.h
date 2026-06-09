#pragma once

#include "model.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace model {

template <typename Archive>
void serialize(Archive& ar, Position& position, [[maybe_unused]] const unsigned version) {
    ar & position.x;
    ar & position.y;
}

template <typename Archive>
void serialize(Archive& ar, Speed& speed, [[maybe_unused]] const unsigned version) {
    ar & speed.dx;
    ar & speed.dy;
}

template <typename Archive>
void serialize(Archive& ar, LostObject& object, [[maybe_unused]] const unsigned version) {
    ar & object.id;
    ar & object.type;
    ar & object.position;
}

template <typename Archive>
void serialize(Archive& ar, Dog::CollectedObject& object, [[maybe_unused]] const unsigned version) {
    ar & object.id;
    ar & object.type;
}

}  // namespace model

namespace serialization {

class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog)
        : id_(dog.GetId())
        , name_(dog.GetName())
        , position_(dog.GetPosition())
        , speed_(dog.GetSpeed())
        , direction_(dog.GetDirection())
        , bag_(dog.GetBag())
        , score_(dog.GetScore()) {
    }

    [[nodiscard]] model::Dog Restore() const {
        model::Dog dog{id_, name_, position_};
        dog.SetSpeed(speed_);
        dog.SetDirection(direction_);
        dog.AddScore(score_);

        for (const auto& item : bag_) {
            dog.AddToBag(model::LostObject{
                .id = item.id,
                .type = item.type,
                .position = {}
            });
        }

        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & id_;
        ar & name_;
        ar & position_;
        ar & speed_;
        ar & direction_;
        ar & bag_;
        ar & score_;
    }

private:
    model::Dog::Id id_ = 0;
    std::string name_;
    model::Position position_;
    model::Speed speed_;
    model::Direction direction_ = model::Direction::NORTH;
    model::Dog::Bag bag_;
    int score_ = 0;
};

class SessionRepr {
public:
    SessionRepr() = default;

    explicit SessionRepr(const model::GameSession& session)
        : map_id_(*session.GetMap().GetId())
        , next_dog_id_(session.GetNextDogId())
        , next_lost_object_id_(session.GetNextLostObjectId()) {
        for (const auto& [id, dog] : session.GetDogs()) {
            dogs_.emplace_back(dog);
        }

        for (const auto& [id, object] : session.GetLostObjects()) {
            lost_objects_.push_back(object);
        }
    }

    void Restore(model::Game& game) const {
        const model::Map* map = game.FindMap(model::Map::Id{map_id_});

        if (!map) {
            throw std::runtime_error("Map not found while restoring state");
        }

        model::GameSession& session = game.FindOrCreateSession(*map);

        for (const DogRepr& dog_repr : dogs_) {
            session.AddRestoredDog(dog_repr.Restore());
        }

        for (const model::LostObject& object : lost_objects_) {
            session.AddRestoredLostObject(object);
        }

        session.SetNextDogId(next_dog_id_);
        session.SetNextLostObjectId(next_lost_object_id_);
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar & map_id_;
        ar & dogs_;
        ar & lost_objects_;
        ar & next_dog_id_;
        ar & next_lost_object_id_;
    }

private:
    std::string map_id_;
    std::vector<DogRepr> dogs_;
    std::vector<model::LostObject> lost_objects_;
    model::Dog::Id next_dog_id_ = 0;
    model::LostObject::Id next_lost_object_id_ = 0;
};

}  // namespace serialization
