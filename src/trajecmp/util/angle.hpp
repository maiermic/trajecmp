#ifndef TRAJECMP_ANGLE_HPP
#define TRAJECMP_ANGLE_HPP

#include <boost/geometry/util/math.hpp>

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

}}

#endif //TRAJECMP_ANGLE_HPP
