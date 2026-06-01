#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "../src/algorithms.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <numbers>
#include <string>
#include <vector>

using namespace std::literals;

using Catch::Matchers::Contains;
using Catch::Matchers::Predicate;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
using Catch::Matchers::WithinULP;

template <typename Range>
struct IsPermutationMatcher : Catch::Matchers::MatcherGenericBase {
    IsPermutationMatcher(Range range)
        : range_{std::move(range)} {
        std::sort(std::begin(range_), std::end(range_));
    }

    IsPermutationMatcher(IsPermutationMatcher&&) = default;

    template <typename OtherRange>
    bool match(OtherRange other) const {
        using std::begin;
        using std::end;

        std::sort(begin(other), end(other));

        return std::equal(begin(range_), end(range_), begin(other), end(other));
    }

    std::string describe() const override {
        return "Is permutation of: "s + Catch::rangeToString(range_);
    }

private:
    Range range_;
};

template <typename Range>
IsPermutationMatcher<Range> IsPermutation(Range&& range) {
    return IsPermutationMatcher<Range>{std::forward<Range>(range)};
}

TEST_CASE("Exceptions can be checked", "[exceptions]") {
    CHECK_THROWS(GetMax<int>({}));
    CHECK_THROWS_AS(GetMax<int>({}), std::invalid_argument);
    CHECK_THROWS_WITH(GetMax<int>({}), "Attempt to get max of empty array"s);

    CHECK_NOTHROW(GetMax<int>({1, 2, 3}));
    CHECK(GetMax<int>({1, 2, 3}) == 3);
}

TEST_CASE("Container matchers make checks readable", "[matchers]") {
    std::vector<int> values = {1, 2, 3, 4, 5};

    CHECK_THAT(values, Contains(3));
    CHECK_THAT(values, !Contains(100));
    CHECK_THAT(values, Contains(3) && !Contains(100));

    auto greater_than_ten = [](int value) {
        return value > 10;
    };

    CHECK_THAT(values, !Contains(Predicate<int>(greater_than_ten)));
}

TEST_CASE("Custom matcher checks permutation", "[custom-matcher]") {
    std::vector<int> values = {3, 1, 2};

    CHECK_THAT(values, IsPermutation(std::vector{1, 2, 3}));
}

TEST_CASE("Floating point values should be compared approximately", "[floating-point]") {
    double value = 0.1;

    CHECK_FALSE(value + value + value + value + value + value + value + value + value + value == 1.0);

    CHECK_THAT(value * 10, WithinAbs(1.0, 1e-15));
    CHECK_THAT(value * 10, WithinRel(1.0, 1e-12));

    CHECK_FALSE(std::asin(0.5) == std::numbers::pi / 6);
    CHECK_THAT(std::asin(0.5), WithinULP(std::numbers::pi / 6, 1));
}

TEST_CASE("Mock comparator counts sort comparisons", "[mock]") {
    const std::size_t size = 1000;

    auto values = GetRandomVector(size);

    CountingComparator<int> comparator;

    std::sort(values.begin(), values.end(), std::ref(comparator));

    CHECK(std::is_sorted(values.begin(), values.end()));
    CHECK(comparator.GetCounter() < 2 * size * std::log2(size));
}
