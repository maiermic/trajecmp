#include <iostream>
#include <vector>

#include <catch.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/all.hpp>

#include "matchers.hpp"

TEST_CASE("rotate using qvm", "[]") {
    SECTION("example") {
        using vec3 = boost::qvm::vec<double, 3>;
        using quat = boost::qvm::quat<double>;
        const vec3 axis{0.0, 1.0, 0.0};
        const auto rotationAngle = (M_PI / 2.0);

        const quat q = boost::qvm::rot_quat(axis, rotationAngle);
        const vec3 rotateVec = q * vec3{1.0, 0.0, 0.0};
        std::vector<double> rotatePos{
                boost::qvm::X(rotateVec),
                boost::qvm::Y(rotateVec),
                boost::qvm::Z(rotateVec)
        };

        std::vector<double> expected{0.0, 0.0, -1.0};
        CHECK_THAT(rotatePos, EqualsApprox(expected));
    }
}