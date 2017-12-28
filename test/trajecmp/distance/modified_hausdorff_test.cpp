#include <catch.hpp>
#include <boost/geometry.hpp>

#include "trajecmp/distance/modified_hausdorff.hpp"

#include "trajecmp/util/boost_geometry_to_string.hpp"

namespace bg = boost::geometry;

using point = bg::model::point<double, 2, bg::cs::cartesian>;
using linestring = bg::model::linestring<point>;
using multi_linestring = bg::model::multi_linestring<linestring>;
using Trajectory = linestring;

TEST_CASE("modified_hausdorff", "[BoostGeometry]") {
    trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff = trajecmp::distance::modified_hausdorff(neighbours);

    SECTION("distance of same trajectory is zero") {
        Trajectory t1 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
        Trajectory t2 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};

        CHECK(0.0 == modified_hausdorff(t1, t2));
    }
    SECTION("distance is maximum") {
        Trajectory trajectory {{0.0, 0.0}, {6.0, 0.0}};
        Trajectory single_point_trajectory {{4.0, 3.0}};
        // equals distance of (0,0) and (4,3)
        CHECK(5.0 == modified_hausdorff(single_point_trajectory, trajectory));
    }
    SECTION("distance is maximum of neighbours") {
        Trajectory t1 {{0, 0}, {10, 0}};
        Trajectory t2 {{0, 0}, {4, 3}, {8, 0}};
        CHECK(10 == bg::length(t1));
        CHECK(10 == bg::length(t2));
        // equals distance of (4,0) and (4,3)
        // (4,3) is at position 50% of t2
        // [(4,0), (6,0)] are the neighbours on t1 at position 50% with max distance of 10%
        CHECK(3.0 == modified_hausdorff(t2, t1));
    }
}
