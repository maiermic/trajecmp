#ifndef TRAJECMP_GEOMETRY_POINT_OPERATIONS_HPP
#define TRAJECMP_GEOMETRY_POINT_OPERATIONS_HPP

#include <type_traits>
#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <trajecmp/trait/concept.hpp>

namespace trajecmp { namespace geometry { namespace point {

    template<
            typename Point,
            typename = std::enable_if_t<trajecmp::trait::is_point<Point>>
    >
    Point operator-(const Point lhs, const Point rhs) {
        Point result(lhs);
        boost::geometry::subtract_point(result, rhs);
        return result;
    }

    template<
            typename Point,
            typename = std::enable_if_t<trajecmp::trait::is_point<Point>>
    >
    Point operator+(const Point lhs, const Point rhs) {
        Point result(lhs);
        boost::geometry::add_point(result, rhs);
        return result;
    }

    template<
            typename Point,
            typename = std::enable_if_t<trajecmp::trait::is_point<Point>>
    >
    Point operator*(const Point lhs, const Point rhs) {
        Point result(lhs);
        boost::geometry::multiply_point(result, rhs);
        return result;
    }

    template<
            typename Point,
            typename = std::enable_if_t<trajecmp::trait::is_point<Point>>
    >
    Point operator/(const Point lhs, const Point rhs) {
        Point result(lhs);
        boost::geometry::divide_point(result, rhs);
        return result;
    }

}}}// namespace trajecmp::geometry::point

#endif //TRAJECMP_GEOMETRY_POINT_OPERATIONS_HPP
