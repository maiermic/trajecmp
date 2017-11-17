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

using trajecmp::util::r2d;
using trajecmp::util::d2r;
using trajecmp::model::trajectory2d;
using trajecmp::model::point2d;
using trajecmp::range::range;
using circle_trajectory = trajecmp::trajectory::circle<trajectory2d>;
using Angle = typename boost::geometry::coordinate_type<trajectory2d>::type;


void check_circle_segment(Angle start_angle,
                          Angle end_angle,
                          const double radius,
                          const point2d center_position) {
    static const circle_trajectory circle_generator(radius);
    using namespace trajecmp::gesture;
    const trajectory2d circle_trajectory =
            trajecmp::transform::translate_by(center_position)(
                    circle_generator.sample(start_angle, end_angle, 10.0)
            );
    auto min_bounding_sphere =
            trajecmp::geometry::min_bounding_sphere(circle_trajectory);
    min_bounding_sphere.center = center_position;
    const circle_segment_info<Angle> c =
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
    CAPTURE(center_position);
    CHECK(c.start_angle == Approx(d2r(start_angle)));

    INFO(r2d(c.end_angle) << " == " << end_angle);
    INFO(c.end_angle << " == " << d2r(end_angle));
    CAPTURE(start_angle);
    CAPTURE(end_angle);
    CAPTURE(min_bounding_sphere);
    CAPTURE(center_position);
    CHECK(c.end_angle == Approx(d2r(end_angle)));
}


TEST_CASE("trajecmp::gesture::estimate_circle_segment", "[]") {
    const double min_angle = -1080.0;
    const double max_angle = 1080.0;
    const double angle_step_size = 10.0;
    const double small_radius = 0.000001;
    const double medium_radius = 100.0;
    const double large_radius = 1000000.0;
    const std::vector<double> radii {
            small_radius,
            medium_radius,
            large_radius
    };
    static const point2d translated_center(12.0, 34.0);
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

TEST_CASE("trajecmp::gesture::estimate_circle_center", "[]") {
    using namespace trajecmp::gesture;
    SECTION("example") {
        using trajecmp::geometry::point::x;
        using trajecmp::geometry::point::y;
        const auto center = estimate_circle_center(point2d(-3.0, 4.0),
                                                   point2d(4.0, 5.0),
                                                   point2d(1.0, -4.0));
        CHECK(x(center) == 1.0);
        CHECK(y(center) == 1.0);
    }
}