#include <vector>

#include <catch.hpp>

#include "matchers.hpp"

TEST_CASE("EqualsApprox", "[]") {
    SECTION("passes") {
        std::vector<double> actual {0, 1.00001};
        std::vector<double> expected {0, 1};
        CHECK_THAT(actual, EqualsApprox(expected));
    }
    SECTION("fails") {
        std::vector<double> actual {0, 1.0001};
        std::vector<double> expected {0, 1};
        CHECK_THAT(actual, !EqualsApprox(expected));
    }
}