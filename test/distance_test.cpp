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
    distance::neighbours_percentage_range neighbours(0.1);

    SECTION("distance of same trajectory is zero") {
        Trajectory t1 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
        Trajectory t2 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};

        CHECK(0.0 == distance::modified_hausdorff(t1, t2, neighbours));
    }
    SECTION("distance is maximum") {
        Trajectory trajectory {{0.0, 0.0}, {6.0, 0.0}};
        Trajectory single_point_trajectory {{4.0, 3.0}};
        // equals distance of (0,0) and (4,3)
        CHECK(5.0 == distance::modified_hausdorff(single_point_trajectory, trajectory, neighbours));
    }
    SECTION("distance is maximum of neighbours") {
        Trajectory t1 {{0, 0}, {10, 0}};
        Trajectory t2 {{0, 0}, {4, 3}, {8, 0}};
        CHECK(10 == bg::length(t1));
        CHECK(10 == bg::length(t2));
        // equals distance of (4,0) and (4,3)
        // (4,3) is at position 50% of t2
        // [(4,0), (6,0)] are the neighbours on t1 at position 50% with max distance of 10%
        CHECK(3.0 == distance::modified_hausdorff(t2, t1, neighbours));
    }
}

TEST_CASE("point_at_percentage_between", "[detail]") {
    point start {4.0, 8.0};
    point end {20.0, 20.0};
    point between = distance::detail::point_at_percentage_between(start, end, 0.25);
    point expected {8.0, 11.0};
    CHECK(expected == between);
}

TEST_CASE("boost::geometry::equals bug fix", "[bug fix]") {
    SECTION("empty trajectory equals empty trajectory") {
        Trajectory t1{};
        Trajectory t2{};
        REQUIRE(t1 == t2);
    }
    SECTION("single point trajectory equals single point trajectory") {
        Trajectory t1{{0,0}};
        Trajectory t2{{0,0}};
        REQUIRE(t1 == t2);
    }
}

TEST_CASE("sub_trajectory", "[detail]") {
    const Trajectory trajectory{{-1, 8},
                                {4,  8},   // length +  5
                                {20, 20},  // length + 20
                                {20, 25},  // length +  5
                                {90, 25}}; // length + 70
    REQUIRE(100 == bg::length(trajectory));
    SECTION("sub-trajectory of empty trajectory is empty") {
        const Trajectory empty {};
        const Trajectory sub_trajectory = distance::detail::sub_trajectory(empty, 0.25, 0.3);
        CHECK(empty == sub_trajectory);
    }
    SECTION("sub-trajectory of single point trajectory equals trajectory") {
        const Trajectory single_point_trajectory {{42, 13}};
        const Trajectory sub_trajectory = distance::detail::sub_trajectory(single_point_trajectory, 0.25, 0.3);
        CHECK(single_point_trajectory == sub_trajectory);
    }
    SECTION("sub-trajectory equals trajectory") {
        const Trajectory all = distance::detail::sub_trajectory(trajectory, 0.0, 1.0);
        CHECK(trajectory == all);
    }
    SECTION("sub-trajectory is segment of trajectory") {
        SECTION("where segment is first segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.0, 0.05);
            const Trajectory expected{trajectory[0],
                                      trajectory[1]};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where segment is middle segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.25, 0.3);
            const Trajectory expected{trajectory[2],
                                      trajectory[3]};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where segment is last segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.3, 1.0);
            const Trajectory expected{trajectory[3],
                                      trajectory[4]};
            CHECK(expected == sub_trajectory);
        }
    }
    SECTION("sub-trajectory is part of a segment of trajectory") {
        SECTION("where segment is first segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.02, 0.03);
            const Trajectory expected{{1, 8},
                                      {2, 8}};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where segment is middle segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.26, 0.28);
            const Trajectory expected{{20, 21},
                                      {20, 23}};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where segment is last segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.90, 1.00);
            const Trajectory expected{{80.0, 25.0},
                                      trajectory[4]};
            CHECK(expected == sub_trajectory);
        }
    }
    SECTION("sub-trajectory is part of multiple segments of trajectory") {
        SECTION("where first point of sub-trajectory is first point of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.0, 0.2);
            const Trajectory expected{trajectory[0],
                                      trajectory[1],
                                      {16, 17}};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where first point of sub-trajectory is first point of a segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.05, 0.2);
            const Trajectory expected{trajectory[1],
                                      {16, 17}};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where sub-trajectory begins in between one segment and ends in between another segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.02, 0.28);
            const Trajectory expected{{1, 8},
                                      trajectory[1],
                                      trajectory[2],
                                      {20, 23}};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where last point of sub-trajectory is last point of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.02, 1.0);
            const Trajectory expected{{1, 8},
                                      trajectory[1],
                                      trajectory[2],
                                      trajectory[3],
                                      trajectory[4]};
            CHECK(expected == sub_trajectory);
        }
        SECTION("where last point of sub-trajectory is last point of a segment of trajectory") {
            const Trajectory sub_trajectory = distance::detail::sub_trajectory(trajectory, 0.02, 0.25);
            const Trajectory expected{{1, 8},
                                      trajectory[1],
                                      trajectory[2]};
            CHECK(expected == sub_trajectory);
        }
    }
}