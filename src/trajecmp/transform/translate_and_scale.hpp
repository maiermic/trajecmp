#ifndef TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP
#define TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>

#include <trajecmp/geometry/point/point.hpp>
#include "map_coordinates.hpp"

namespace trajecmp { namespace transform {

    template<
            typename Trajectory,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type,
            typename Scale = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    void translate_and_scale(const Vector &vector,
                             Scale scale,
                             Trajectory &trajectory) {
        using Point = Vector;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        for (Point &point : trajectory) {
            map_coordinates_with_index(point, [&](std::size_t i, Coordinate c) {
                using trajecmp::geometry::point::get;
                return (c + get(i, vector)) * scale;
            });
        }
    };

    template<
            typename Trajectory,
            typename Scale = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Vector = typename boost::geometry::point_type<Trajectory>::type
    >
    void scale_and_translate(Scale scale,
                             const Vector &vector,
                             Trajectory &trajectory) {
        using Point = Vector;
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        for (Point &point : trajectory) {
            map_coordinates_with_index(point, [&](std::size_t i, Coordinate c) {
                using trajecmp::geometry::point::get;
                return c * scale + get(i, vector);
            });
        }
    };

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_TRANSLATE_AND_SCALE_HPP
