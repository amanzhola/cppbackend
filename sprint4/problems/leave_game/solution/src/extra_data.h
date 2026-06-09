#pragma once

#include "model.h"

#include <boost/json.hpp>

#include <cstdint>
#include <unordered_map>

namespace extra_data {

namespace json = boost::json;

class ExtraData {
public:
    void AddLootTypes(const model::Map::Id& map_id, json::array loot_types) {
        loot_types_by_map_id_[map_id] = std::move(loot_types);
    }

    const json::array& GetLootTypes(const model::Map::Id& map_id) const {
        static const json::array empty;

        if (auto it = loot_types_by_map_id_.find(map_id); it != loot_types_by_map_id_.end()) {
            return it->second;
        }

        return empty;
    }

    int GetLootValue(const model::Map::Id& map_id, std::uint32_t type) const {
        const auto& loot_types = GetLootTypes(map_id);

        if (type >= loot_types.size()) {
            return 0;
        }

        const auto& loot_type = loot_types.at(type).as_object();

        if (!loot_type.contains("value")) {
            return 0;
        }

        return static_cast<int>(loot_type.at("value").as_int64());
    }

private:
    std::unordered_map<model::Map::Id, json::array, util::TaggedHasher<model::Map::Id>>
        loot_types_by_map_id_;
};

}  // namespace extra_data
