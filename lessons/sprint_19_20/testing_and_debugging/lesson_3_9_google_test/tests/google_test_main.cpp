#include <gtest/gtest.h>

#include "../src/leap_year.h"

TEST(LeapYearTestSuite, LeapYearIsDivisibleBy4) {
    ASSERT_TRUE(IsLeapYear(2020));
    ASSERT_FALSE(IsLeapYear(2021));
    ASSERT_FALSE(IsLeapYear(2022));
    ASSERT_FALSE(IsLeapYear(2023));
    ASSERT_TRUE(IsLeapYear(2024));
}

TEST(LeapYearTestSuite, LeapYearIsNotDivisibleBy100ButIsDivisibleBy400) {
    ASSERT_FALSE(IsLeapYear(1900));
    ASSERT_TRUE(IsLeapYear(2000));
}
