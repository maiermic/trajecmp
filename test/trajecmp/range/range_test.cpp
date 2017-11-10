#include <vector>

#include <catch.hpp>

#include <boost/range.hpp>

#include "trajecmp/range/range.hpp"

using namespace trajecmp::range;


template <class Range, class Value = boost::iterator_value<Range>>
void check_range(std::initializer_list<Value> expected, Range actual) {
    CHECK((std::vector<Value>(expected)) ==
          std::vector<Value>(boost::begin(actual), boost::end(actual)));
};

TEST_CASE("trajecmp::range::range", "[range]") {
    // int
    SECTION("range step default (int)") {
        check_range({0, 1, 2}, range(0, 2));
    }
    SECTION("range increasing step 1 (int)") {
        check_range({0, 1, 2}, range(0, 2, 1));
    }
    SECTION("range increasing step 2 (int)") {
        check_range({0, 2}, range(0, 2, 2));
    }
    SECTION("range decreasing step 1 (int)") {
        check_range({0, -1, -2}, range(0, -2, 1));
    }
    SECTION("range decreasing step 2 (int)") {
        check_range({0, -2}, range(0, -2, 2));
    }
    SECTION("range decreasing step -1 (int)") {
        check_range({0, -1, -2}, range(0, -2, -1));
    }
    SECTION("range step 0 throws exception (int)") {
        CHECK_THROWS(range(0, 2, 0));
    }
    // float
    SECTION("range step default (float)") {
        check_range({0.0f, 1.0f, 2.0f}, range(0.0f, 2.0f));
    }
    SECTION("range increasing step 1.0f (float)") {
        check_range({0.0f, 1.0f, 2.0f}, range(0.0f, 2.0f, 1.0f));
    }
    SECTION("range increasing step 2.0f (float)") {
        check_range({0.0f, 2.0f}, range(0.0f, 2.0f, 2.0f));
    }
    SECTION("range decreasing step 1 (float)") {
        check_range({0.0f, -1.0f, -2.0f}, range(0.0f, -2.0f, 1.0f));
    }
    SECTION("range decreasing step 2.0f (float)") {
        check_range({0.0f, -2.0f}, range(0.0f, -2.0f, 2.0f));
    }
    SECTION("range decreasing step -1.0f (float)") {
        check_range({0.0f, -1.0f, -2.0f}, range(0.0f, -2.0f, -1.0f));
    }
    SECTION("range step 0 throws exception (float)") {
        CHECK_THROWS(range(0.0f, 2.0f, 0.0f));
    }
    // double
    SECTION("range step default (double)") {
        check_range({0.0, 1.0, 2.0}, range(0.0, 2.0));
    }
    SECTION("range increasing step 1.0 (double)") {
        check_range({0.0, 1.0, 2.0}, range(0.0, 2.0, 1.0));
    }
    SECTION("range increasing step 2.0 (double)") {
        check_range({0.0, 2.0}, range(0.0, 2.0, 2.0));
    }
    SECTION("range decreasing step 1 (double)") {
        check_range({0.0, -1.0, -2.0}, range(0.0, -2.0, 1.0));
    }
    SECTION("range decreasing step 2.0 (double)") {
        check_range({0.0, -2.0}, range(0.0, -2.0, 2.0));
    }
    SECTION("range decreasing step -1.0 (double)") {
        check_range({0.0, -1.0, -2.0}, range(0.0, -2.0, -1.0));
    }
    SECTION("range step 0 throws exception (double)") {
        CHECK_THROWS(range(0.0, 2.0, 0.0));
    }
}