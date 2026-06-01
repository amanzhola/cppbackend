#define _USE_MATH_DEFINES

#include <catch2/catch_test_macros.hpp>

#include "../src/collision_detector.h"

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

using namespace std::literals;

namespace cd = collision_detector;

namespace Catch {

template <>
struct StringMaker<cd::GatheringEvent> {
    static std::string convert(const cd::GatheringEvent& value) {
        std::ostringstream out;

        out << "("
            << "gatherer_id=" << value.gatherer_id
            << ", item_id=" << value.item_id
            << ", sq_distance=" << value.sq_distance
            << ", time=" << value.time
            << ")";

        return out.str();
    }
};

}  // namespace Catch

namespace {

constexpr double EPSILON = 1e-10;

class TestItemGathererProvider : public cd::ItemGathererProvider {
public:
    TestItemGathererProvider(std::vector<cd::Item> items, std::vector<cd::Gatherer> gatherers)
        : items_{std::move(items)}
        , gatherers_{std::move(gatherers)} {
    }

    size_t ItemsCount() const override {
        return items_.size();
    }

    cd::Item GetItem(size_t idx) const override {
        return items_.at(idx);
    }

    size_t GatherersCount() const override {
        return gatherers_.size();
    }

    cd::Gatherer GetGatherer(size_t idx) const override {
        return gatherers_.at(idx);
    }

private:
    std::vector<cd::Item> items_;
    std::vector<cd::Gatherer> gatherers_;
};

bool IsClose(double lhs, double rhs) {
    return std::abs(lhs - rhs) <= EPSILON;
}

void CheckEvent(const cd::GatheringEvent& actual,
                size_t expected_item_id,
                size_t expected_gatherer_id,
                double expected_sq_distance,
                double expected_time) {
    CHECK(actual.item_id == expected_item_id);
    CHECK(actual.gatherer_id == expected_gatherer_id);
    CHECK(IsClose(actual.sq_distance, expected_sq_distance));
    CHECK(IsClose(actual.time, expected_time));
}

}  // namespace

SCENARIO("FindGatherEvents detects collisions") {
    GIVEN("one gatherer moving along X axis and several items") {
        TestItemGathererProvider provider{
            {
                cd::Item{{2.0, 0.0}, 0.5},    // collected, time = 0.2
                cd::Item{{5.0, 0.75}, 0.5},   // collected, sq_distance = 0.75^2
                cd::Item{{5.0, 1.01}, 0.5},   // not collected, too far
                cd::Item{{-0.1, 0.0}, 0.5},   // not collected, before start
                cd::Item{{10.1, 0.0}, 0.5},   // not collected, after finish
            },
            {
                cd::Gatherer{{0.0, 0.0}, {10.0, 0.0}, 0.5},
            }
        };

        WHEN("gather events are searched") {
            const auto events = cd::FindGatherEvents(provider);

            THEN("only real collisions are detected") {
                REQUIRE(events.size() == 2);

                CheckEvent(events[0], 0, 0, 0.0, 0.2);
                CheckEvent(events[1], 1, 0, 0.75 * 0.75, 0.5);
            }
        }
    }
}

SCENARIO("FindGatherEvents does not detect extra events") {
    GIVEN("items outside movement segment or outside collection radius") {
        TestItemGathererProvider provider{
            {
                cd::Item{{-1.0, 0.0}, 0.1},
                cd::Item{{11.0, 0.0}, 0.1},
                cd::Item{{5.0, 2.0}, 0.1},
            },
            {
                cd::Gatherer{{0.0, 0.0}, {10.0, 0.0}, 0.1},
            }
        };

        WHEN("gather events are searched") {
            const auto events = cd::FindGatherEvents(provider);

            THEN("no events are returned") {
                CHECK(events.empty());
            }
        }
    }

    GIVEN("a gatherer that did not move") {
        TestItemGathererProvider provider{
            {
                cd::Item{{0.0, 0.0}, 10.0},
                cd::Item{{1.0, 1.0}, 10.0},
            },
            {
                cd::Gatherer{{0.0, 0.0}, {0.0, 0.0}, 10.0},
            }
        };

        WHEN("gather events are searched") {
            const auto events = cd::FindGatherEvents(provider);

            THEN("no events are returned") {
                CHECK(events.empty());
            }
        }
    }
}

SCENARIO("FindGatherEvents returns events in chronological order") {
    GIVEN("several gatherers and items with different collision times") {
        TestItemGathererProvider provider{
            {
                cd::Item{{2.0, 0.3}, 0.25},   // gatherer 0, time = 0.2, sq = 0.09
                cd::Item{{7.0, 0.0}, 0.25},   // gatherer 0, time = 0.7, sq = 0
                cd::Item{{0.4, 5.0}, 0.25},   // gatherer 1, time = 0.5, sq = 0.16
            },
            {
                cd::Gatherer{{0.0, 0.0}, {10.0, 0.0}, 0.25},
                cd::Gatherer{{0.0, 10.0}, {0.0, 0.0}, 0.25},
            }
        };

        WHEN("gather events are searched") {
            const auto events = cd::FindGatherEvents(provider);

            THEN("events are sorted by collision time") {
                REQUIRE(events.size() == 3);

                CheckEvent(events[0], 0, 0, 0.3 * 0.3, 0.2);
                CheckEvent(events[1], 2, 1, 0.4 * 0.4, 0.5);
                CheckEvent(events[2], 1, 0, 0.0, 0.7);

                CHECK(events[0].time < events[1].time);
                CHECK(events[1].time < events[2].time);
            }
        }
    }
}

SCENARIO("FindGatherEvents correctly handles boundary collisions") {
    GIVEN("items exactly on start, finish and collection radius boundary") {
        TestItemGathererProvider provider{
            {
                cd::Item{{0.0, 0.0}, 0.0},    // start point, time = 0
                cd::Item{{10.0, 0.0}, 0.0},   // finish point, time = 1
                cd::Item{{5.0, 1.0}, 0.5},    // exactly at radius 1.0
            },
            {
                cd::Gatherer{{0.0, 0.0}, {10.0, 0.0}, 0.5},
            }
        };

        WHEN("gather events are searched") {
            const auto events = cd::FindGatherEvents(provider);

            THEN("boundary contacts are counted as collisions") {
                REQUIRE(events.size() == 3);

                CheckEvent(events[0], 0, 0, 0.0, 0.0);
                CheckEvent(events[1], 2, 0, 1.0, 0.5);
                CheckEvent(events[2], 1, 0, 0.0, 1.0);
            }
        }
    }
}
