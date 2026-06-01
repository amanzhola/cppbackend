#include <catch2/catch_test_macros.hpp>

#include "../src/math_tools.h"

TEST_CASE("Sqr calculates square", "[math]") {
    CHECK(Sqr(3) == 9);
    CHECK(Sqr(-5) == 25);
    CHECK(Sqr(0) == 0);
}

TEST_CASE("IsEven checks even numbers", "[math]") {
    CHECK(IsEven(2));
    CHECK(IsEven(0));
    CHECK_FALSE(IsEven(3));
    CHECK_FALSE(IsEven(-7));
}
