#include <ostream>
#include <utility>
#include <vector>

namespace std {

    template<class F, class S>
    ostream &operator<<(ostream &out, const pair<F, S> &p) {
        out << '(' << p.first << ", " << p.second << ')';
        return out;
    }

}

#include <catch.hpp>

#include "trajecmp/algorithm/minmax_index.hpp"

using namespace trajecmp::algorithm;

TEST_CASE("trajecmp::algorithm::minmax_index", "[]") {
    using Values = std::vector<int>;
    SECTION("returns index of searched element") {
        CHECK(std::make_pair(3l, 0l) == minmax_index(Values {8, 2, 7, 1}));
        CHECK(std::make_pair(1l, 2l) == minmax_index(Values {2, 1, 8, 7}));
        CHECK(std::make_pair(0l, 0l) == minmax_index(Values {1}));
        CHECK(std::make_pair(0l, 1l) == minmax_index(Values {1, 1}));
    }
    SECTION("returns index of last found element") {
        CHECK(std::make_pair(1l, 2l) == minmax_index(Values {8, 1, 8, 1}));
        CHECK(std::make_pair(0l, 3l) == minmax_index(Values {1, 8, 1, 8}));
    }
    SECTION("returns -1 as indices if container is empty") {
        CHECK(std::make_pair(-1l, -1l) == minmax_index(Values {}));
    }
}

TEST_CASE("trajecmp::algorithm::min_index", "[]") {
    using Values = std::vector<int>;
    SECTION("returns index of searched element") {
        CHECK(3l == min_index(Values {8, 2, 7, 1}));
        CHECK(1l == min_index(Values {2, 1, 8, 7}));
        CHECK(0l == min_index(Values {1}));
        CHECK(0l == min_index(Values {1, 1}));
    }
    SECTION("returns index of last found min-element") {
        CHECK(1l == min_index(Values {8, 1, 8, 1}));
        CHECK(0l == min_index(Values {1, 8, 1, 8}));
    }
    SECTION("returns -1 as indices if container is empty") {
        CHECK(-1l == min_index(Values {}));
    }
}

TEST_CASE("trajecmp::algorithm::max_index", "[]") {
    using Values = std::vector<int>;
    SECTION("returns index of searched element") {
        CHECK(0l == max_index(Values {8, 2, 7, 1}));
        CHECK(2l == max_index(Values {2, 1, 8, 7}));
        CHECK(0l == max_index(Values {1}));
        CHECK(0l == max_index(Values {1, 1}));
    }
    SECTION("returns index of first found max-element") {
        CHECK(0l == max_index(Values {8, 1, 8, 1}));
        CHECK(1l == max_index(Values {1, 8, 1, 8}));
    }
    SECTION("returns -1 as indices if container is empty") {
        CHECK(-1l == max_index(Values {}));
    }
}