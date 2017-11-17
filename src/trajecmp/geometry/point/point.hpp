#ifndef TRAJECMP_GEOMETRY_POINT_HPP
#define TRAJECMP_GEOMETRY_POINT_HPP

#include <functional>
#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include "boost/geometry/core/coordinate_type.hpp"
#include <trajecmp/trait/number_type_trait.hpp>

namespace trajecmp { namespace geometry { namespace point {

    namespace detail {

        template<std::size_t>
        struct int2type {
        };


        template<class Point, std::size_t I>
        typename boost::geometry::coordinate_type<Point>::type
        get_imp(std::size_t index, const Point &point, int2type<I>) {
            return (I == index)
                   ? boost::geometry::get<I>(point)
                   : get_imp(index, point, int2type<I - 1>());
        }

        template<class Point>
        typename boost::geometry::coordinate_type<Point>::type
        get_imp(std::size_t index, const Point &point, int2type<0>) {
            return boost::geometry::get<0>(point);
        }


        template<class Point, std::size_t I>
        void set_imp(std::size_t index,
                     Point &point,
                     typename boost::geometry::coordinate_type<Point>::type value,
                     int2type<I>) {
            return (I == index)
                   ? boost::geometry::set<I>(point, value)
                   : set_imp(index, point, value, int2type<I - 1>());
        }

        template<class Point>
        void set_imp(std::size_t index,
                     Point &point,
                     typename boost::geometry::coordinate_type<Point>::type value,
                     int2type<0>) {
            return boost::geometry::set<0>(point, value);
        }

    } // namespace detail


    template<class Point>
    typename boost::geometry::coordinate_type<Point>::type
    get(std::size_t index, const Point &point) {
        static std::size_t const size = boost::geometry::dimension<Point>::value;
        return detail::get_imp(index, point, detail::int2type<size - 1>());
    }

    template<class Point>
    void set(std::size_t index, Point &point,
             typename boost::geometry::coordinate_type<Point>::type value) {
        static std::size_t const size = boost::geometry::dimension<Point>::value;
        return detail::set_imp(index, point, value,
                               detail::int2type<size - 1>());
    }

    template<class Point>
    typename boost::geometry::coordinate_type<Point>::type
    x(const Point &point) {
      return boost::geometry::get<0>(point);
    }

    template<class Point>
    typename boost::geometry::coordinate_type<Point>::type
    y(const Point &point) {
      return boost::geometry::get<1>(point);
    }

    template<class Point>
    typename boost::geometry::coordinate_type<Point>::type
    z(const Point &point) {
      return boost::geometry::get<2>(point);
    }

    template<class Point>
    Point to_point(
            const typename boost::geometry::coordinate_type<Point>::type *point) {
        const auto dimension = boost::geometry::dimension<Point>::value;
        Point result;
        for (int i = 0; i < dimension; ++i, ++point) {
            set(i, result, *point);
        }
        return result;
    }

    template<class Point>
    bool equals_approx(const Point &lhs, const Point &rhs) {
        using CoordinateType = typename boost::geometry::coordinate_type<Point>::type;
        const auto eps =
                trajecmp::trait::number_type_trait<CoordinateType>::get_default_eps();
        const auto DimensionCount = boost::geometry::dimension<Point>::value;
        for(std::size_t i = 0; i < DimensionCount; i++) {
            const auto coordinate_difference = get(i, lhs) - get(i, rhs);
            if (coordinate_difference > eps ||
                coordinate_difference < -eps) {
                return false;
            }
        }
        return true;
    }

    template<class Point>
    Point zero() {
        Point p;
        boost::geometry::assign_zero(p);
        return p;
    }

    template<class Point>
    bool equals_approx_zero(const Point &p) {
        return equals_approx(p, zero<Point>());
    }

}}}// namespace trajecmp::geometry::point

#endif //TRAJECMP_GEOMETRY_POINT_HPP
