#ifndef TRAJECMP_GEOMETRY_HYPER_SPHERE_HPP
#define TRAJECMP_GEOMETRY_HYPER_SPHERE_HPP

#include <functional>

namespace trajecmp { namespace geometry {

    template<class Point, class Radius>
    struct hyper_sphere {
        Point center;
        Radius radius = 0;
    };

    template<class Point, class Radius>
    std::ostream &operator<<(std::ostream &os, const hyper_sphere<Point, Radius> &sphere) {
        os << "{ center: " << sphere.center << ", radius: " << sphere.radius << " }";
        return os;
    }

    template<class Geometry>
    using hyper_sphere_of = hyper_sphere<
            typename boost::geometry::point_type<Geometry>::type,
            typename boost::geometry::coordinate_type<Geometry>::type
    >;

}}// namespace trajecmp::geometry

#endif //TRAJECMP_GEOMETRY_HYPER_SPHERE_HPP
