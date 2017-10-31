#ifndef TRAJECMP_ANGLE_HPP
#define TRAJECMP_ANGLE_HPP

#include <boost/geometry/arithmetic/dot_product.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/util/math.hpp>
#include <boost/qvm/vec_operations.hpp>

#include <trajecmp/trait/number_type_trait.hpp>
#include <trajecmp/geometry/point/point.hpp>

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

}}

#endif //TRAJECMP_ANGLE_HPP
