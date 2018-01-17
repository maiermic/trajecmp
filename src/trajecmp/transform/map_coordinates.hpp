#ifndef TRAJECMP_TRANSFORM_MAP_COORDINATES_HPP
#define TRAJECMP_TRANSFORM_MAP_COORDINATES_HPP

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>

#include <trajecmp/geometry/point/point.hpp>

namespace trajecmp { namespace transform {

    template<typename Point, typename F>
    void map_coordinates_with_index(Point &point, const F f) {
        using trajecmp::geometry::point::get;
        using trajecmp::geometry::point::set;
        constexpr auto dimension = boost::geometry::dimension<Point>::value;
        for (std::size_t i = 0; i < dimension; ++i) {
            set(i, point, f(i, get(i, point)));
        }
    };

    template<typename Point, typename F>
    void map_coordinates(Point &point, const F f) {
        using trajecmp::geometry::point::get;
        using trajecmp::geometry::point::set;
        constexpr auto dimension = boost::geometry::dimension<Point>::value;
        for (std::size_t i = 0; i < dimension; ++i) {
            set(i, point, f(get(i, point)));
        }
    };

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_MAP_COORDINATES_HPP
