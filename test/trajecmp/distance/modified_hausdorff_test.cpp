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

TEST_CASE("modified_hausdorff_info", "[BoostGeometry]") {
    trajecmp::distance::neighbours_percentage_range neighbours(0.1);
    const auto modified_hausdorff_info =
            trajecmp::distance::modified_hausdorff_info(neighbours);

    SECTION("distance of same trajectory is zero") {
        const Trajectory t1 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};
        const Trajectory t2 {{0.0, 0.0}, {1.0, 0.0}, {1.0, 2.0}};

        auto d = modified_hausdorff_info(t1, t2);
        CHECK(0.0 == d.real_distance);
        CHECK(d.projected_point1 == point(0.0, 0.0));
        CHECK(d.projected_point2 == point(0.0, 0.0));
    }
    SECTION("distance is maximum") {
        const Trajectory trajectory {{0.0, 0.0}, {6.0, 0.0}};
        const Trajectory single_point_trajectory {{4.0, 3.0}};
        // equals distance of (0,0) and (4,3)
        auto d = modified_hausdorff_info(trajectory, single_point_trajectory);
        CHECK(5.0 == d.real_distance);
        CHECK(d.projected_point1 == point(0.0, 0.0));
        CHECK(d.projected_point2 == point(4.0, 3.0));
        d = modified_hausdorff_info(single_point_trajectory, trajectory);
        CHECK(5.0 == d.real_distance);
        CHECK(d.projected_point1 == point(4.0, 3.0));
        CHECK(d.projected_point2 == point(0.0, 0.0));
    }
    SECTION("distance is maximum of neighbours") {
        const Trajectory t1 {{0, 0}, {10, 0}};
        const Trajectory t2 {{0, 0}, {4, 3}, {8, 0}};
        CHECK(10 == bg::length(t1));
        CHECK(10 == bg::length(t2));
        // equals distance of (4,0) and (4,3)
        // (4,3) is at position 50% of t2
        // [(4,0), (6,0)] are the neighbours on t1 at position 50% with max distance of 10%
        auto d = modified_hausdorff_info(t1, t2);
        CHECK(3.0 == d.real_distance);
        CHECK(d.projected_point1 == point(4.0, 0.0));
        CHECK(d.projected_point2 == point(4.0, 3.0));
        d = modified_hausdorff_info(t2, t1);
        CHECK(3.0 == d.real_distance);
        CHECK(d.projected_point1 == point(4.0, 3.0));
        CHECK(d.projected_point2 == point(4.0, 0.0));
    }
    SECTION("distance of real world example") {
        const Trajectory rectangle_pattern{
                {-4.03813, -2.94848},
                {-4.03813, 3.46125},
                {4.03813,  3.46125},
                {4.03813,  -2.94848},
                {-4.03813, -2.94848}
        };
        const Trajectory rectangle_input{
                {-3.69628, -2.82028},
                {-3.69628, -0.384584},
                {-4.03813, 2.94848},
                {0.40595,  3.46125},
                {1.00419,  3.1194},
                {2.92711,  3.16213},
                {2.96984,  1.58107},
                {4.03813,  -2.94848},
                {-3.69628, -2.82028}
        };
        const double rectangle_pattern_length = 28.972;
        CHECK(bg::length(rectangle_pattern) == Approx(rectangle_pattern_length));
        const double rectangle_input_length = 26.8432;
        CHECK(bg::length(rectangle_input) == Approx(rectangle_input_length));
        const bg::distance_info_result<point> d = modified_hausdorff_info(rectangle_input, rectangle_pattern);
        CHECK(d.real_distance == Approx(1.1505816854));
        CHECK(d.real_distance == Approx(bg::distance(d.projected_point1, d.projected_point2)));
        CHECK(d.projected_point1 == point(2.92711, 3.16213));
        CHECK(d.projected_point2 == point(4.03813, 3.46125));
    }
}
