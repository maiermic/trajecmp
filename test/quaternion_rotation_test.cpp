#include <trajecmp/transform/rotate.hpp>

#include <iostream>
#include <vector>

#include <catch.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/all.hpp>
#include <boost/geometry.hpp>

#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>

#include "matchers.hpp"

struct point3d {
    point3d(double x, double y, double z) : x(x), y(y), z(z) {}
    point3d() : point3d(0, 0, 0) {}

    double x;
    double y;
    double z;
};
BOOST_GEOMETRY_REGISTER_POINT_3D(point3d, double, boost::geometry::cs::cartesian, x, y, z);
using point2d = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

using trajectory2d = std::vector<point2d>;
using trajectory3d = std::vector<point3d>;
BOOST_GEOMETRY_REGISTER_LINESTRING(trajectory2d);
BOOST_GEOMETRY_REGISTER_LINESTRING(trajectory3d);

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

namespace Catch {
    template<>
    struct StringMaker<point2d> {
        static std::string convert(point2d const &p) {
            return Catch::toString(std::vector<double> {
                    boost::geometry::get<0>(p),
                    boost::geometry::get<1>(p),
            });
        }
    };
    template<>
    struct StringMaker<point3d> {
        static std::string convert(point3d const &p) {
            return Catch::toString(std::vector<double> {p.x, p.y, p.z});
        }
    };
}

TEST_CASE("rotate using qvm", "[rotate]") {
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

TEST_CASE("rotate 2D trajectory", "[rotate]") {
    const trajectory2d input{
            {1, 0},
            {0, 1},
            {1, 1},
    };
    using namespace trajecmp::transform;
    SECTION("90 degree, where degree is used as default") {
        CHECK_THAT(
                rotate(90)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        {0, -1},
                        {1, 0},
                        {1, -1},
                })
        );
    }
    SECTION("180 degree") {
        CHECK_THAT(
                rotate<degree>(180)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        {-1, 0},
                        {0,  -1},
                        {-1, -1},
                })
        );
    }
    SECTION("90 degree in radians") {
        CHECK_THAT(
                rotate<radian>(M_PI / 2.0)(input),
                TrajectoryEqualsApprox(trajectory2d {
                        {0, -1},
                        {1, 0},
                        {1, -1},
                })
        );
    }
}

TEST_CASE("rotate 3D trajectory", "[rotate]") {
    const trajectory3d input{
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1},
            {1, 1, 0},
            {1, 0, 1},
            {0, 1, 1},
            {1, 1, 1},
    };
    using namespace trajecmp::transform;
    SECTION("90 degree in radians") {
        using quat = boost::qvm::quat<double>;
        const point3d axis{0.0, 1.0, 0.0};
        const auto rotationAngle = (M_PI / 2.0);

        const quat q = boost::qvm::rot_quat(axis, rotationAngle);
        CHECK_THAT(
                rotate_using_quaternion(q)(input),
                TrajectoryEqualsApprox(trajectory3d {
                        {0, 0, -1},
                        {0, 1, 0},
                        {1, 0, 0},
                        {0, 1, -1},
                        {1, 0, -1},
                        {1, 1, 0},
                        {1, 1, -1},
                })
        );
    }
}
