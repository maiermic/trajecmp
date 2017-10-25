#include <iostream>
#include <vector>

#include <catch.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/all.hpp>
#include <boost/geometry.hpp>

#include <boost/geometry/geometries/register/point.hpp>

#include "matchers.hpp"

struct point3d {
    point3d(double x, double y, double z) : x(x), y(y), z(z) {}
    point3d() : point3d(0, 0, 0) {}

    double x;
    double y;
    double z;
};

BOOST_GEOMETRY_REGISTER_POINT_3D(point3d, double, boost::geometry::cs::cartesian, x, y, z);

namespace boost { namespace qvm {
    template<>
    struct vec_traits<point3d> {
        static int const dim = 3;
        using scalar_type = double;

        template<int I>
        static inline scalar_type &write_element(point3d &v) {
            switch (I) {
                case 0: return v.x;
                case 1: return v.y;
                case 2: return v.z;
                default: throw std::runtime_error("invalid index");
            }
        }

        template<int I>
        static inline scalar_type
        read_element(point3d const &v) {
            switch (I) {
                case 0: return v.x;
                case 1: return v.y;
                case 2: return v.z;
                default: throw std::runtime_error("invalid index");
            }
        }
    };
}}

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
    SECTION("example using Boost Geometry point") {
        using quat = boost::qvm::quat<double>;
        const point3d axis{0.0, 1.0, 0.0};
        const auto rotationAngle = (M_PI / 2.0);

        const quat q = boost::qvm::rot_quat(axis, rotationAngle);
        const point3d rotateVec = q * point3d{1.0, 0.0, 0.0};
        std::vector<double> rotatePos{
                boost::geometry::get<0>(rotateVec),
                boost::geometry::get<1>(rotateVec),
                boost::geometry::get<2>(rotateVec)
        };

        std::vector<double> expected{0.0, 0.0, -1.0};
        CHECK_THAT(rotatePos, EqualsApprox(expected));
    }
}