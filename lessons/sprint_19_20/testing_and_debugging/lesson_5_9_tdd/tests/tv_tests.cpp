#define BOOST_TEST_MODULE TV tests
#include <boost/test/included/unit_test.hpp>

#include "../src/tv.h"

BOOST_AUTO_TEST_SUITE(TV_)

BOOST_AUTO_TEST_CASE(is_off_by_default) {
    TV tv;

    BOOST_CHECK(!tv.IsTurnedOn());
}

BOOST_AUTO_TEST_SUITE_END()
