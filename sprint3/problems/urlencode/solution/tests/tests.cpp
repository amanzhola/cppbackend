#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, EmptyString) {
    EXPECT_EQ(UrlEncode(""sv), ""s);
}

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
    EXPECT_EQ(UrlEncode("abcXYZ123-._~"sv), "abcXYZ123-._~"s);
}

TEST(UrlEncodeTestSuite, SpacesAreEncodedAsPlus) {
    EXPECT_EQ(UrlEncode("Hello World"sv), "Hello+World"s);
    EXPECT_EQ(UrlEncode("a b c"sv), "a+b+c"s);
}

TEST(UrlEncodeTestSuite, ReservedSymbolsArePercentEncoded) {
    EXPECT_EQ(UrlEncode("Hello World!"sv), "Hello+World%21"s);
    EXPECT_EQ(UrlEncode("abc*"sv), "abc%2A"s);
    EXPECT_EQ(UrlEncode("a/b?c=d&e"sv), "a%2Fb%3Fc%3Dd%26e"s);
}

TEST(UrlEncodeTestSuite, ControlCharactersArePercentEncoded) {
    EXPECT_EQ(UrlEncode(std::string_view{"a\nb", 3}), "a%0Ab"s);
    EXPECT_EQ(UrlEncode(std::string_view{"\x01\x1F", 2}), "%01%1F"s);
}

TEST(UrlEncodeTestSuite, CharactersWithCode128AndHigherArePercentEncoded) {
    const std::string input{static_cast<char>(128), static_cast<char>(255)};
    EXPECT_EQ(UrlEncode(input), "%80%FF"s);
}
