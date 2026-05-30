#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

using namespace std::literals;

TEST_CASE("String without html entities") {
    CHECK(HtmlDecode("Hello world"sv) == "Hello world"s);
    CHECK(HtmlDecode("Johnson&Johnson"sv) == "Johnson&Johnson"s);
}

TEST_CASE("Empty string") {
    CHECK(HtmlDecode(""sv) == ""s);
}

TEST_CASE("Lowercase html entities with semicolon") {
    CHECK(HtmlDecode("&lt;"sv) == "<"s);
    CHECK(HtmlDecode("&gt;"sv) == ">"s);
    CHECK(HtmlDecode("&amp;"sv) == "&"s);
    CHECK(HtmlDecode("&apos;"sv) == "'"s);
    CHECK(HtmlDecode("&quot;"sv) == "\""s);
}

TEST_CASE("Lowercase html entities without semicolon") {
    CHECK(HtmlDecode("&lt"sv) == "<"s);
    CHECK(HtmlDecode("&gt"sv) == ">"s);
    CHECK(HtmlDecode("&amp"sv) == "&"s);
    CHECK(HtmlDecode("&apos"sv) == "'"s);
    CHECK(HtmlDecode("&quot"sv) == "\""s);
}

TEST_CASE("Uppercase html entities") {
    CHECK(HtmlDecode("&LT;"sv) == "<"s);
    CHECK(HtmlDecode("&GT;"sv) == ">"s);
    CHECK(HtmlDecode("&AMP;"sv) == "&"s);
    CHECK(HtmlDecode("&APOS;"sv) == "'"s);
    CHECK(HtmlDecode("&QUOT;"sv) == "\""s);
}

TEST_CASE("Mixed case entities are not decoded") {
    CHECK(HtmlDecode("&aPos;"sv) == "&aPos;"s);
    CHECK(HtmlDecode("&Amp;"sv) == "&Amp;"s);
    CHECK(HtmlDecode("&QuOt;"sv) == "&QuOt;"s);
}

TEST_CASE("Entities at beginning middle and end") {
    CHECK(HtmlDecode("&lt;tag&gt;"sv) == "<tag>"s);
    CHECK(HtmlDecode("M&amp;M&APOSs"sv) == "M&M's"s);
    CHECK(HtmlDecode("&quot;hello&quot;"sv) == "\"hello\""s);
}

TEST_CASE("Unknown entities are kept unchanged") {
    CHECK(HtmlDecode("&abracadabra"sv) == "&abracadabra"s);
    CHECK(HtmlDecode("hello&unknown;world"sv) == "hello&unknown;world"s);
}

TEST_CASE("Decoded symbols are not decoded again") {
    CHECK(HtmlDecode("&amp;lt;"sv) == "&lt;"s);
    CHECK(HtmlDecode("&AMPamp;"sv) == "&amp;"s);
}

TEST_CASE("Incomplete entities") {
    CHECK(HtmlDecode("&"sv) == "&"s);
    CHECK(HtmlDecode("&l"sv) == "&l"s);
    CHECK(HtmlDecode("&ap"sv) == "&ap"s);
    CHECK(HtmlDecode("&quo"sv) == "&quo"s);
}

TEST_CASE("Examples from statement") {
    CHECK(HtmlDecode("Johnson&amp;Johnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&ampJohnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&AMP;Johnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&AMPJohnson"sv) == "Johnson&Johnson"s);
    CHECK(HtmlDecode("Johnson&Johnson"sv) == "Johnson&Johnson"s);
}
