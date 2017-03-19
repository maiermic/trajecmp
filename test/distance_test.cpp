#include <catch.hpp>
#include <boost/geometry.hpp>

#include "src/distance.h"

#include "boost_geometry_to_string.h"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using multi_linestring = bg::model::multi_linestring<linestring>;
using Trajectory = linestring;

TEST_CASE("modified_hausdorff", "[BoostGeometry]") {
    SECTION("distance of same trajectory is zero") {
        Trajectory t1 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
        Trajectory t2 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};

        CAPTURE(t1);
        CAPTURE(t2);
        CHECK(0.0 == distance::modified_hausdorff(t1, t2));
    }
}
