#include <trajecmp/model/quaternion.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/model/trajectory.hpp>
#include <trajecmp/transform/rotate.hpp>

#include <catch.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/all.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/core/access.hpp>

#include "../../matchers.hpp"

using trajecmp::model::point2d;
using trajecmp::model::point3d;
using trajecmp::model::trajectory2d;
using trajecmp::model::trajectory3d;

TEST_CASE("rotate 2D trajectory", "[rotate]") {
    const trajectory2d input{
            point2d(1.0, 0.0),
            point2d(0.0, 1.0),
            point2d(1.0, 1.0),
    };
    using namespace trajecmp::transform;
    SECTION("90 degree, where degree is used as default") {
        CHECK_THAT(
                rotate(90.0)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        point2d(0.0, -1.0),
                        point2d(1.0, 0.0),
                        point2d(1.0, -1.0),
                })
        );
    }
    SECTION("180 degree") {
        CHECK_THAT(
                rotate<degree>(180.0)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        point2d(-1.0, 0.0),
                        point2d(0.0,  -1.0),
                        point2d(-1.0, -1.0),
                })
        );
    }
    SECTION("90 degree in radians") {
        CHECK_THAT(
                rotate<radian>(M_PI / 2.0)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        point2d(0.0, -1.0),
                        point2d(1.0, 0.0),
                        point2d(1.0, -1.0),
                })
        );
    }
}

TEST_CASE("rotate 3D trajectory", "[rotate]") {
    const trajectory3d input{
            point3d(1.0, 0.0, 0.0),
            point3d(0.0, 1.0, 0.0),
            point3d(0.0, 0.0, 1.0),
            point3d(1.0, 1.0, 0.0),
            point3d(1.0, 0.0, 1.0),
            point3d(0.0, 1.0, 1.0),
            point3d(1.0, 1.0, 1.0),
    };
    using namespace trajecmp::transform;
    SECTION("90 degree in radians") {
        const point3d axis{0.0, 1.0, 0.0};
        const auto rotationAngle = (M_PI / 2.0);

        const auto q = trajecmp::model::quaternion(axis, rotationAngle);
        CHECK_THAT(
                rotate_using_quaternion(q)(input),
                TrajectoryEqualsApprox(trajectory3d {
                        point3d(0.0, 0.0, -1.0),
                        point3d(0.0, 1.0, 0.0),
                        point3d(1.0, 0.0, 0.0),
                        point3d(0.0, 1.0, -1.0),
                        point3d(1.0, 0.0, -1.0),
                        point3d(1.0, 1.0, 0.0),
                        point3d(1.0, 1.0, -1.0),
                })
        );
    }
    SECTION("from one vector to another") {
        using trajecmp::model::quaternion;
        const point3d x_axis{1.0, 0.0, 0.0};
        const point3d z_axis{0.0, 0.0, 1.0};
        CHECK_THAT(
                rotate_using_quaternion(quaternion(x_axis, z_axis))(input),
                TrajectoryEqualsApprox(trajectory3d {
                        point3d(0.0, 0.0, -1.0),
                        point3d(0.0, 1.0, 0.0),
                        point3d(1.0, 0.0, 0.0),
                        point3d(0.0, 1.0, -1.0),
                        point3d(1.0, 0.0, -1.0),
                        point3d(1.0, 1.0, 0.0),
                        point3d(1.0, 1.0, -1.0),
                })
        );
    }
}
