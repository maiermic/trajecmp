#ifndef TRAJECMP_GEOMETRY_MIN_BOUNDING_BOX_HPP
#define TRAJECMP_GEOMETRY_MIN_BOUNDING_BOX_HPP

#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

namespace trajecmp { namespace geometry {

    template<typename Geometry>
    using box_type_of = boost::geometry::model::box<typename boost::geometry::point_type<Geometry>::type>;

    template<class Geometry>
    box_type_of<Geometry> min_bounding_box(const Geometry &geometry) {
        namespace bg = boost::geometry;
        return bg::return_envelope<box_type_of<Geometry>>(geometry);
    }

}} // namespace trajecmp::geometry

#endif //TRAJECMP_GEOMETRY_MIN_BOUNDING_BOX_HPP
