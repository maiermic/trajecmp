#include <trajecmp/gesture/circle.hpp>
#include <trajecmp/trajectory/circle.hpp>
#include <trajecmp/model/trajectory.hpp>
#include <trajecmp/range/range.hpp>

#include <catch.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>

using trajecmp::util::r2d;
using trajecmp::model::trajectory2d;
using trajecmp::model::point2d;
using trajecmp::range::range;
using circle_trajectory = trajecmp::trajectory::circle<trajectory2d>;
using Angle = typename boost::geometry::coordinate_type<trajectory2d>::type;


void check_circle_segment(Angle start_angle,
                          Angle end_angle,
                          const double radius) {
    static const circle_trajectory circle_generator(radius);
    using namespace trajecmp::gesture;
    const auto circle_trajectory =
            circle_generator.sample(start_angle, end_angle, 10.0);
    auto min_bounding_sphere =
            trajecmp::geometry::min_bounding_sphere(circle_trajectory);
    min_bounding_sphere.center = point2d(0.0, 0.0);
    const circle_segment_info<Angle> c =
            estimate_circle_segment(
                    circle_trajectory,
                    min_bounding_sphere
            );
    INFO("check_circle_segment(" << start_angle << ", " << end_angle << ")");
    INFO(r2d(c.start_angle) << " == " << start_angle);
    CAPTURE(min_bounding_sphere);
    CHECK(r2d(c.start_angle) == Approx(start_angle));

    INFO("check_circle_segment(" << start_angle << ", " << end_angle << ")");
    INFO(r2d(c.end_angle) << " == " << end_angle);
    CAPTURE(min_bounding_sphere);
    CHECK(r2d(c.end_angle) == Approx(end_angle));
}


TEST_CASE("trajecmp::gesture::estimate_circle_segment", "[]") {
    SECTION("radius of 100, start angle of 0 and end angle between -1080 and 1080 with step size of 10") {
        for (double end_angle : range(-1080.0, 1080.0, 10.0)) {
            check_circle_segment(0.0, end_angle, 100.0);
        }
    }
    SECTION("radius of 1000000, start angle of 0 and end angle between -1080 and 1080 with step size of 10") {
        for (double end_angle : range(-1080.0, 1080.0, 10.0)) {
            check_circle_segment(0.0, end_angle, 1000000.0);
        }
    }
    SECTION("radius of 0.000001, start angle of 0 and end angle between -1080 and 1080 with step size of 10") {
        for (double end_angle : range(-1080.0, 1080.0, 10.0)) {
            check_circle_segment(0.0, end_angle, 0.000001);
        }
    }
}