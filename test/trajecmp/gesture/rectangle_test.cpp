#include <trajecmp/gesture/rectangle.hpp>
#include <trajecmp/geometry/point/point.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/model/trajectory.hpp>
#include <trajecmp/range/to_trajectory.hpp>
#include <trajecmp/util/vector_ostream.hpp>

#include <catch.hpp>

using trajecmp::model::trajectory2d;
using trajecmp::model::point2d;
using trajecmp::gesture::estimate_rectangle_corners;
using index_type = trajectory2d::difference_type;
using trajecmp::gesture::rectangle_corner_indices;
using trajecmp::geometry::point::x;
using trajecmp::geometry::point::y;
using boost::geometry::return_envelope;
using box = boost::geometry::model::box<point2d>;

const point2d min_corner(-0.5, -0.5);
const point2d max_corner(0.5, 0.5);
const point2d min_x_max_y_corner(x(min_corner), y(max_corner));
const point2d max_x_min_y_corner(x(max_corner), y(min_corner));
const trajectory2d rectangle_clockwise_from_min_corner{
        min_corner,
        min_x_max_y_corner,
        max_corner,
        max_x_min_y_corner,
        min_corner,
};
const trajectory2d &normalized_rectangle_trajectory = rectangle_clockwise_from_min_corner;
const trajectory2d rectangle_counterclockwise_from_min_corner =
        ranges::view::reverse(rectangle_clockwise_from_min_corner) |
        trajecmp::range::to_trajectory<trajectory2d>();
const trajectory2d rectangle_clockwise_from_max_corner{
        max_corner,
        max_x_min_y_corner,
        min_corner,
        min_x_max_y_corner,
        max_corner,
};
const trajectory2d rectangle_counterclockwise_from_max_corner =
        ranges::view::reverse(rectangle_clockwise_from_max_corner) |
        trajecmp::range::to_trajectory<trajectory2d>();

TEST_CASE("trajecmp::gesture::estimate_rectangle_corners", "[]") {
    SECTION("rectangle clockwise from min corner") {
        CHECK(estimate_rectangle_corners(
                return_envelope<box>(rectangle_clockwise_from_min_corner),
                rectangle_clockwise_from_min_corner) ==
              (rectangle_corner_indices<index_type> {0, 1, 2, 3}));
    }
    SECTION("rectangle counterclockwise from min corner") {
        CHECK(estimate_rectangle_corners(
                return_envelope<box>(
                        rectangle_counterclockwise_from_min_corner),
                rectangle_counterclockwise_from_min_corner) ==
              (rectangle_corner_indices<index_type> {0, 3, 2, 1}));
    }
    SECTION("rectangle clockwise from max corner") {
        CHECK(estimate_rectangle_corners(
                return_envelope<box>(rectangle_clockwise_from_max_corner),
                rectangle_clockwise_from_max_corner) ==
              (rectangle_corner_indices<index_type> {2, 3, 4, 1}));
    }
    SECTION("rectangle counterclockwise from max corner") {
        CHECK(estimate_rectangle_corners(
                return_envelope<box>(
                        rectangle_counterclockwise_from_max_corner),
                rectangle_counterclockwise_from_max_corner) ==
              (rectangle_corner_indices<index_type> {2, 1, 4, 3}));
    }
}

TEST_CASE("trajecmp::gesture::get_corner_after_min_corner", "[]") {
    SECTION("rectangle clockwise from min corner") {
        CHECK(1 == get_corner_after_min_corner(
                rectangle_corner_indices<index_type> {0, 1, 2, 3}));
    }
    SECTION("rectangle counterclockwise from min corner") {
        CHECK(1 == get_corner_after_min_corner(
                rectangle_corner_indices<index_type> {0, 3, 2, 1}));
    }
    SECTION("rectangle clockwise from max corner") {
        CHECK(3 == get_corner_after_min_corner(
                rectangle_corner_indices<index_type> {2, 3, 4, 1}));
    }
    SECTION("rectangle counterclockwise from max corner") {
        CHECK(3 == get_corner_after_min_corner(
                rectangle_corner_indices<index_type> {2, 1, 4, 3}));
    }
}

template<class Trajectory>
Trajectory normalize_rectangle_trajectory(const Trajectory &trajectory) {
    return get_normalized_rectangle_trajectory(
            estimate_rectangle_corners(return_envelope<box>(trajectory),
                                       trajectory),
            trajectory
    );
}

TEST_CASE("trajecmp::gesture::get_normalized_rectangle_trajectory", "[]") {
    SECTION("rectangle clockwise from min corner") {
        CHECK(normalized_rectangle_trajectory == normalize_rectangle_trajectory(
                rectangle_clockwise_from_min_corner));
    }
    SECTION("rectangle counterclockwise from min corner") {
        CHECK(normalized_rectangle_trajectory == normalize_rectangle_trajectory(
                rectangle_counterclockwise_from_min_corner));
    }
    SECTION("rectangle clockwise from max corner") {
        CHECK(normalized_rectangle_trajectory == normalize_rectangle_trajectory(
                rectangle_clockwise_from_max_corner));
    }
    SECTION("rectangle counterclockwise from max corner") {
        CHECK(normalized_rectangle_trajectory == normalize_rectangle_trajectory(
                rectangle_counterclockwise_from_max_corner));
    }
}
