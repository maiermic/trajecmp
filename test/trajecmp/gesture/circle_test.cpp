#include <functional>

#include <trajecmp/util/vector_ostream.hpp>
#include "../../../example/logging.hpp"
#include <trajecmp/gesture/circle.hpp>
#include <trajecmp/transform/translate.hpp>
#include <trajecmp/trajectory/circle.hpp>
#include <trajecmp/model/trajectory.hpp>
#include <trajecmp/range/range.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>

#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/common_type.hpp>

#include <catch.hpp>
#include "../../matchers.hpp"

using trajecmp::util::r2d;
using trajecmp::util::d2r;
using trajecmp::model::trajectory2d;
using trajecmp::model::point2d;
using trajecmp::range::range;
using circle_trajectory = trajecmp::trajectory::circle<trajectory2d>;
using Angle = typename boost::geometry::coordinate_type<trajectory2d>::type;


void check_circle_segment(const Angle start_angle,
                          const Angle end_angle,
                          const double radius,
                          const point2d center) {
    const circle_trajectory circle_generator(radius);
    using namespace trajecmp::gesture;
    const trajectory2d circle_trajectory =
            trajecmp::transform::translate_by(center)(
                    circle_generator.sample(start_angle, end_angle, 1.0)
            );
    if (circle_trajectory.size() < 3) return; // requires more than 3 points
    const auto min_bounding_sphere =
            trajecmp::geometry::min_bounding_sphere(circle_trajectory);
    const circle_segment_info<Angle, point2d> c =
            estimate_circle_segment(
                    circle_trajectory,
                    min_bounding_sphere
            );
    INFO(r2d(c.start_angle) << " == " << start_angle);
    INFO(c.start_angle << " == " << d2r(start_angle));
    CAPTURE(c.start_angle);
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(min_bounding_sphere);
    CAPTURE(c.center);
    CAPTURE(center);
    CHECK(c.start_angle == Approx(d2r(start_angle)));

    INFO(r2d(c.end_angle) << " == " << end_angle);
    INFO(c.end_angle << " == " << d2r(end_angle));
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(min_bounding_sphere);
    CAPTURE(c.center);
    CAPTURE(center);
    CHECK(c.end_angle == Approx(d2r(end_angle)));

    INFO(c.center << " == " << center);
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(min_bounding_sphere);
    CAPTURE(c.center);
    CAPTURE(center);
    CAPTURE(c.center == center);
    CHECK(point_equals_approx(c.center, center));

    INFO(c.center << " == " << center);
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(min_bounding_sphere);
    CAPTURE(c.radius);
    CAPTURE(c.center);
    CAPTURE(center);
    CAPTURE(c.center == center);
    CHECK(c.radius == Approx(radius));
}


TEST_CASE("trajecmp::gesture::estimate_circle_segment", "[]") {
    const double min_angle = -1080.0;
    const double max_angle = 1080.0;
    const double angle_step_size = 10.0;
    const double small_radius = 0.001;
    const double medium_radius = 100.0;
    const double large_radius = 1000000.0;
    const std::vector<double> radii {
            small_radius,
            medium_radius,
            large_radius
    };
    const point2d translated_center(12.0, 34.0);
    const std::vector<point2d> center_points{
            translated_center,
            point2d(0.0, 0.0)
    };
    const auto full_circle_angles = range(min_angle, max_angle, 360.0);
    using namespace std::placeholders;
    using ranges::view::cartesian_product;
    using ranges::view::transform;
    const auto angles_near_full_circle =
            cartesian_product(full_circle_angles, range(-9.0, 9.0, 1.0)) |
            transform(std::bind(ranges::tuple_apply, std::plus<double>(), _1));
    SECTION("start_angle < 0 < end_angle, clockwise winding direction") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                check_circle_segment(-20.0, 10.0, radius, center);
                check_circle_segment(-20.0, 360.0, radius, center);
                check_circle_segment(-20.0, 370.0, radius, center);
                check_circle_segment(-20.0, 270.0, radius, center);
            }
        }
    }
    SECTION("start_angle < 0 = end_angle, clockwise winding direction") {
        check_circle_segment(-20.0, 0.0, small_radius, translated_center);
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                check_circle_segment(-20.0, 0.0, radius, center);
            }
        }
    }
    SECTION("start_angle > 0 > end_angle, counterclockwise winding direction") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                check_circle_segment(10.0, -20.0, radius, center);
                check_circle_segment(10.0, -380.0, radius, center);
            }
        }
    }
    SECTION("start_angle = 0 < end_angle, counterclockwise winding direction") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                check_circle_segment(0.0, -20.0, radius, center);
                check_circle_segment(0.0, -380.0, radius, center);
            }
        }
    }
    SECTION("0 > start_angle > end_angle, counterclockwise winding direction") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                check_circle_segment(-30.0, -300.0, radius, center);
                check_circle_segment(-30.0, -660.0, radius, center);
            }
        }
    }
    SECTION("generated parameters") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                for (const double end_angle : range(min_angle, max_angle, angle_step_size)) {
                    check_circle_segment(0.0, end_angle, radius, center);
                }
            }
        }
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                for (const double end_angle : angles_near_full_circle) {
                    check_circle_segment(0.0, end_angle, radius, center);
                }
            }
        }
    }
}

void check_circle_center(const Angle start_angle,
                         const Angle end_angle,
                         const double radius,
                         const point2d center_position) {
    const circle_trajectory circle_generator(radius);
    using trajecmp::geometry::point::x;
    using trajecmp::geometry::point::y;
    using trajecmp::geometry::point::equals_approx;
    using namespace trajecmp::gesture;
    const trajectory2d circle_trajectory =
            trajecmp::transform::translate_by(center_position)(
                    circle_generator.sample(start_angle, end_angle, 5.0)
            );
    REQUIRE(circle_trajectory.size() >= 3);
    const point2d estimated_center =
            trajecmp::gesture::estimate_circle_center(
                circle_trajectory.front(),
                circle_trajectory.at(circle_trajectory.size() / 2),
                circle_trajectory.back()
            );
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(radius);
    CAPTURE(estimated_center);
    CAPTURE(center_position);
    CHECK(point_equals_approx(estimated_center, center_position));
}

TEST_CASE("trajecmp::gesture::estimate_circle_center", "[]") {
    using namespace trajecmp::gesture;
    const double angle_step_size = 10.0;
    const double small_radius = 0.000001;
    const double medium_radius = 100.0;
    const double large_radius = 1000000.0;
    const std::vector<double> radii {
            small_radius,
            medium_radius,
            large_radius
    };
    const point2d translated_center(12.0, 34.0);
    const std::vector<point2d> center_points{
            translated_center,
            point2d(0.0, 0.0)
    };
    SECTION("example") {
        using trajecmp::geometry::point::x;
        using trajecmp::geometry::point::y;
        const auto center = estimate_circle_center(point2d(-3.0, 4.0),
                                                   point2d(4.0, 5.0),
                                                   point2d(1.0, -4.0));
        CAPTURE(center);
        CHECK(point_equals_approx(center, point2d(1.0, 1.0)));
    }
    SECTION("generated parameters") {
        for (const point2d &center : center_points) {
            for (const double radius : radii) {
                for (const double end_angle : range(0.0 + angle_step_size, 360.0 - angle_step_size, angle_step_size)) {
                    check_circle_center(0.0, end_angle, radius, center);
                }
            }
        }
    }
}