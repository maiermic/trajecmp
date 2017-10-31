#include "trajecmp/util/angle.hpp"
#include "trajecmp/model/point.hpp"

#include <catch.hpp>


using namespace trajecmp::util;

TEST_CASE("trajecmp::util::enclosed_angle", "[]") {
    SECTION("2D") {
        using trajecmp::model::point2d;
        const point2d zero{0.0, 0.0};
        const point2d x_axis{1.0, 0.0};
        const point2d y_axis{0.0, 1.0};
        SECTION("is zero if at least one vector is zero") {
            CHECK(enclosed_angle(zero, y_axis) == 0.0);
            CHECK(enclosed_angle(x_axis, zero) == 0.0);
            CHECK(enclosed_angle(zero, zero) == 0.0);
        }
        SECTION("45 degree") {
            CHECK(enclosed_angle(x_axis, point2d(1.0, 1.0)) == Approx(d2r(45.0)));
        }
        SECTION("90 degree") {
            CHECK(enclosed_angle(x_axis, y_axis) == d2r(90.0));
            CHECK(enclosed_angle(y_axis, x_axis) == d2r(90.0));
        }
        SECTION("180 degree") {
            CHECK(enclosed_angle(x_axis, point2d(-1.0, 0.0)) == d2r(180.0));
        }
    }
    SECTION("3D") {
        using trajecmp::model::point3d;
        const point3d zero{0.0, 0.0, 0.0};
        const point3d x_axis{1.0, 0.0, 0.0};
        const point3d y_axis{0.0, 1.0, 0.0};
        const point3d z_axis{0.0, 0.0, 1.0};
        SECTION("is zero if at least one vector is zero") {
            CHECK(enclosed_angle(zero, y_axis) == 0.0);
            CHECK(enclosed_angle(x_axis, zero) == 0.0);
            CHECK(enclosed_angle(zero, zero) == 0.0);
        }
        SECTION("45 degree") {
            CHECK(enclosed_angle(x_axis, point3d(1.0, 1.0, 0.0)) == Approx(d2r(45.0)));
        }
        SECTION("90 degree") {
            CHECK(enclosed_angle(x_axis, y_axis) == d2r(90.0));
            CHECK(enclosed_angle(x_axis, z_axis) == d2r(90.0));
            CHECK(enclosed_angle(y_axis, x_axis) == d2r(90.0));
            CHECK(enclosed_angle(z_axis, x_axis) == d2r(90.0));
            CHECK(enclosed_angle(y_axis, z_axis) == d2r(90.0));
            CHECK(enclosed_angle(z_axis, y_axis) == d2r(90.0));
        }
        SECTION("180 degree") {
            CHECK(enclosed_angle(x_axis, point3d(-1.0, 0.0, 0.0)) == d2r(180.0));
        }
    }
}