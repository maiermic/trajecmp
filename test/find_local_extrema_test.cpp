#include <vector>

#include <catch.hpp>

#include "trajecmp/util/find_local_extrema.hpp"

using namespace trajecmp::util;

TEST_CASE("find_local_extrema", "[]") {
    SECTION("find minima and maxima (double)") {
        const std::vector<double> values{1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5,
                                         6, 7};
        const extrema e = find_local_extrema(values, 0.01);
        CHECK(1 == e.maxima.size());
        CHECK(3 == e.maxima.front());
        CHECK(1 == e.minima.size());
        CHECK(7 == e.minima.front());
    }
    SECTION("find minima and maxima (float)") {
        const std::vector<float> values{1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5,
                                         6, 7};
        const extrema e = find_local_extrema(values, 0.01f);
        CHECK(1 == e.maxima.size());
        CHECK(3 == e.maxima.front());
        CHECK(1 == e.minima.size());
        CHECK(7 == e.minima.front());
    }
}