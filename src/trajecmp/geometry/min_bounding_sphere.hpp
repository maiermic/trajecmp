#ifndef TRAJECMP_GEOMETRY_MIN_BOUNDING_SPHERE_HPP
#define TRAJECMP_GEOMETRY_MIN_BOUNDING_SPHERE_HPP

#include <functional>
#include <ostream>
#include <boost/geometry.hpp>
#include "../../Miniball.hpp"
#include "coordinate.hpp"
#include "hyper_sphere.hpp"
#include "trajecmp/geometry/point/point.hpp"

namespace trajecmp { namespace geometry {

    template<class Geometry>
    auto min_bounding_sphere(const Geometry &geometry) {
        using Point = typename boost::geometry::point_type<Geometry>::type;
        using Miniball = typename Miniball::Miniball<coordinate_accessor<Geometry>>;
        const Miniball miniball(boost::geometry::dimension<Geometry>::value, geometry.begin(), geometry.end());
        return hyper_sphere_of < Geometry > {
                .center = point::to_point<Point>(miniball.center()),
                .radius = std::sqrt(miniball.squared_radius())
        };
    }

}} // namespace trajecmp::geometry

#endif //TRAJECMP_GEOMETRY_MIN_BOUNDING_SPHERE_HPP
