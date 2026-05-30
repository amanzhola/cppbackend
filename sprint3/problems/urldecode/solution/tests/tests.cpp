#define BOOST_TEST_MODULE UrlDecodeTests
#include <boost/test/included/unit_test.hpp>

#include "../src/urldecode.h"

#include <stdexcept>
#include <string>

BOOST_AUTO_TEST_CASE(DecodeEmptyString) {
    BOOST_CHECK_EQUAL(UrlDecode(""), "");
}

BOOST_AUTO_TEST_CASE(DecodeStringWithoutPercentSequences) {
    BOOST_CHECK_EQUAL(UrlDecode("Hello-World_123.~"), "Hello-World_123.~");
    BOOST_CHECK_EQUAL(UrlDecode("Hello World !"), "Hello World !");
}

BOOST_AUTO_TEST_CASE(DecodeValidPercentSequencesInDifferentCases) {
    BOOST_CHECK_EQUAL(UrlDecode("Hello%20World%21"), "Hello World!");
    BOOST_CHECK_EQUAL(UrlDecode("%48%65%6C%6c%6F"), "Hello");
    BOOST_CHECK_EQUAL(UrlDecode("%2f%2F%3a%3A"), "//::");
}

BOOST_AUTO_TEST_CASE(DecodePlusAsSpace) {
    BOOST_CHECK_EQUAL(UrlDecode("Hello+World"), "Hello World");
    BOOST_CHECK_EQUAL(UrlDecode("a+b+c"), "a b c");
    BOOST_CHECK_EQUAL(UrlDecode("+"), " ");
}

BOOST_AUTO_TEST_CASE(DecodeInvalidPercentSequences) {
    BOOST_CHECK_THROW(UrlDecode("%GG"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("%4Z"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("%Z4"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello%XXWorld"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(DecodeIncompletePercentSequences) {
    BOOST_CHECK_THROW(UrlDecode("%"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("%1"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello%"), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello%2"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(DecodeUnescapedReservedCharactersAsIs) {
    BOOST_CHECK_EQUAL(UrlDecode("!#$&'()*%2B,/:;=?@[]"), "!#$&'()*+,/:;=?@[]");
    BOOST_CHECK_EQUAL(UrlDecode("Hello%20World !"), "Hello World !");
}
