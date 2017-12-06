#ifndef TRAJECMP_ANGLE_HPP
#define TRAJECMP_ANGLE_HPP

#include <boost/geometry/arithmetic/dot_product.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/util/math.hpp>
#include <boost/qvm/vec_operations.hpp>

#include <trajecmp/trait/number_type_trait.hpp>
#include <trajecmp/geometry/point/point.hpp>
#include "approx.hpp"

namespace trajecmp { namespace util {

    template <typename T>
    inline T d2r(T degree)
    {
        return (degree * boost::geometry::math::pi<T>()) / T(180.0);
    }

    template <typename T>
    inline T r2d(T rad)
    {
        return (rad * T(180.0)) / boost::geometry::math::pi<T>();
    }

    template<typename Vector>
    typename boost::geometry::coordinate_type<Vector>::type
    enclosed_angle(const Vector &v1, const Vector &v2) {
        namespace bg = boost::geometry;
        using trajecmp::trait::number_type_trait;
        using trajecmp::geometry::point::equals_approx_zero;
        using Value = typename boost::geometry::coordinate_type<Vector>::type;
        const auto zero = number_type_trait<Value>::get_zero_element();
        const auto eps = number_type_trait<Value>::get_default_eps();

        if (equals_approx_zero(v1) || equals_approx_zero(v2)) {
            return zero;
        }
        Value returnValue = bg::dot_product(v1, v2);
        returnValue /= (boost::qvm::mag(v1) * boost::qvm::mag(v2));

        if ((returnValue - eps) < -1.) {
            return bg::math::pi<Value>();
        } else if ((returnValue + eps) > 1.) {
            return zero;
        } else {
            return std::acos(returnValue);
        }
    }

    /**
     * Calculates the smallest angle in radians between two vectors taking into
     * account the direction of rotation.
     * @tparam Vector The type of the vectors.
     * @param v1 The start vector.
     * @param v2 The end vector.
     * @return Value between [-π, π], which corresponds to the interval
     * of degree values [-180, 180].
     * If the smallest angle between the two vectors is clockwise, the angle is
     * a positive value.
     * If the smallest angle between the two vectors is counterclockwise, the
     * angle is a negative value.
     * If one of the vectors is zero or if the vectors are equal, the angle is
     * zero.
     */
    template<typename Vector>
    typename boost::geometry::coordinate_type<Vector>::type
    angle(const Vector &v1, const Vector &v2) {
        namespace bg = boost::geometry;
        using trajecmp::trait::number_type_trait;
        using trajecmp::geometry::point::equals_approx_zero;
        using Value = typename bg::coordinate_type<Vector>::type;
        const auto zero = number_type_trait<Value>::get_zero_element();
        if (equals_approx_zero(v1) || equals_approx_zero(v2)) {
            return zero;
        }
        return std::atan2(bg::get<1>(v1), bg::get<0>(v1)) -
               std::atan2(bg::get<1>(v2), bg::get<0>(v2));
    }

    /**
     * Calculates the smallest clockwise angle in radians between two vectors.
     * @tparam Vector The type of the vectors.
     * @param v1 The start vector.
     * @param v2 The end vector.
     * @return Value between [0, 2π), which corresponds to the interval
     * of degree values [0, 360).
     * If one of the vectors is zero or if the vectors are equal, the angle is
     * zero.
     */
    template<typename Vector>
    typename boost::geometry::coordinate_type<Vector>::type
    angle_clockwise(const Vector &v1, const Vector &v2) {
        namespace bg = boost::geometry;
        using Value = typename bg::coordinate_type<Vector>::type;
        const auto a = angle(v1, v2);
        return a == approx(Value(0))
               ? Value(0)
               : a < 0 ?
                 d2r<Value>(Value(360)) + a : a;
    }

    /**
     * Calculates the smallest counterclockwise angle in radians between two
     * vectors.
     * @tparam Vector The type of the vectors.
     * @param v1 The start vector.
     * @param v2 The end vector.
     * @return Value between [0, 2π), which corresponds to the interval
     * of degree values [0, 360).
     * If one of the vectors is zero or if the vectors are equal, the angle is
     * zero.
     */
    template<typename Vector>
    typename boost::geometry::coordinate_type<Vector>::type
    angle_counterclockwise(const Vector &v1, const Vector &v2) {
        namespace bg = boost::geometry;
        using Value = typename bg::coordinate_type<Vector>::type;
        const auto a = angle(v1, v2);
        return a <= approx(Value(0))
               ? std::abs(a)
               : d2r<Value>(Value(360)) - a;
    }

}}

#endif //TRAJECMP_ANGLE_HPP
