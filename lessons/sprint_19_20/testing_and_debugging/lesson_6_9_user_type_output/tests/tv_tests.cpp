#define BOOST_TEST_MODULE TV tests
#include <boost/test/included/unit_test.hpp>

#include <optional>
#include <ostream>
#include <string_view>

#include "../src/tv.h"

namespace test_util {

std::ostream& operator<<(std::ostream& out, std::nullopt_t) {
    using namespace std::literals;
    return out << "nullopt"sv;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::optional<T>& opt_value) {
    if (opt_value) {
        return out << *opt_value;
    }

    return out << std::nullopt;
}

}  // namespace test_util

namespace boost::test_tools::tt_detail {

template <>
struct print_log_value<std::nullopt_t> {
    void operator()(std::ostream& out, const std::nullopt_t&) {
        using ::test_util::operator<<;
        out << std::nullopt;
    }
};

template <typename T>
struct print_log_value<std::optional<T>> {
    void operator()(std::ostream& out, const std::optional<T>& opt_value) {
        using ::test_util::operator<<;
        out << opt_value;
    }
};

}  // namespace boost::test_tools::tt_detail

BOOST_AUTO_TEST_SUITE(TV_)

BOOST_AUTO_TEST_CASE(is_off_by_default) {
    TV tv;

    BOOST_TEST(!tv.IsTurnedOn());
}

BOOST_AUTO_TEST_CASE(doesnt_show_a_channel_when_it_is_off) {
    TV tv;

    BOOST_TEST(!tv.IsTurnedOn());
    BOOST_TEST(!tv.GetChannel().has_value());
}

BOOST_AUTO_TEST_CASE(optional_printing) {
    using namespace std::literals;
    using test_util::operator<<;

    std::ostringstream output;

    output << std::make_optional(42)
           << ' '
           << std::nullopt
           << ' '
           << std::optional<int>{};

    BOOST_TEST(std::move(output).str() == "42 nullopt nullopt"sv);
}

BOOST_AUTO_TEST_CASE(shows_channel_1_after_turning_on) {
    TV tv;

    tv.TurnOn();

    BOOST_TEST(tv.IsTurnedOn());
    BOOST_TEST(tv.GetChannel() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
