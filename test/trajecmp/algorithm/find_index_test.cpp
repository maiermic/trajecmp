#include <vector>

#include <catch.hpp>

#include "trajecmp/algorithm/find_index.hpp"

using namespace trajecmp::algorithm;

TEST_CASE("trajecmp::algorithm::find_index", "[]") {
    using Values = std::vector<int>;
    SECTION("returns index of searched element") {
        CHECK(0 == find_index(Values {8, 1, 7}, 8));
        CHECK(1 == find_index(Values {1, 8, 7}, 8));
        CHECK(2 == find_index(Values {1, 7, 8}, 8));
    }
    SECTION("returns index of first found element") {
        CHECK(0 == find_index(Values {8, 1, 8}, 8));
        CHECK(1 == find_index(Values {1, 8, 8}, 8));
    }
    SECTION("returns -1 if element is not found") {
        CHECK(-1 == find_index(Values {2, 1, 7}, 8));
    }
}