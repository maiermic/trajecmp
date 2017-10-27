#include <trajecmp/model/quaternion.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/util/angle.hpp>

#include <boost/geometry.hpp>
#include <boost/qvm/all.hpp>
#include <catch.hpp>
#include "../../matchers.hpp"


TEST_CASE("quaternion model", "[rotate]") {
    using trajecmp::model::point3d;
    using trajecmp::model::point3f;
    using trajecmp::model::quaternion;
    using trajecmp::util::d2r;

    SECTION("from one vector to another (double") {
        const point3d x_axis{1.0, 0.0, 0.0};
        const point3d y_axis{0.0, 1.0, 0.0};
        const point3d z_axis{0.0, 0.0, 1.0};

        CHECK_THAT(quat_to_vector(quaternion(x_axis, z_axis)),
                   EqualsApprox(quat_to_vector(quaternion(y_axis, d2r(90.0)))));
        CHECK_THAT(quat_to_vector(quaternion(z_axis, x_axis)),
                   EqualsApprox(quat_to_vector(quaternion(y_axis, d2r(-90.0)))));
    }
}
