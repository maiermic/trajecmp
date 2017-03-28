#ifndef TRAJECTORY_DETECTION_MINBOUNDINGSPHERE_H
#define TRAJECTORY_DETECTION_MINBOUNDINGSPHERE_H

#include <cmath>
#include <boost/geometry.hpp>
#include <ostream>
#include "coordinate_iterator.h"
#include "CoordinateAccessor.h"
#include "Miniball.hpp"

template<class Point>
Point toPoint(const typename boost::geometry::coordinate_type<Point>::type *point) {
    const auto dimension = boost::geometry::dimension<Point>::value;
    Point result;
    for (int i = 0; i < dimension; ++i, ++point) {
        set(i, result, *point);
    }
    return result;
}

template<class Geometry>
class MinBoundingSphere {
    using Point = typename boost::geometry::point_type<Geometry>::type;
    using Radius = typename boost::geometry::coordinate_type<Point>::type;
    using _Miniball = typename Miniball::Miniball<CoordinateAccessor<Geometry>>;

    explicit MinBoundingSphere(const _Miniball miniball)
            : center(toPoint<Point>(miniball.center())),
              radius(std::sqrt(miniball.squared_radius())),
              squared_radius(miniball.squared_radius()) {}

public:
    explicit MinBoundingSphere(const Geometry &geometry)
            : MinBoundingSphere(_Miniball(boost::geometry::dimension<Geometry>::value,
                                          geometry.begin(),
                                          geometry.end())) {}

    const Point center;
    const Radius radius;
    const Radius squared_radius;


    friend std::ostream &operator<<(std::ostream &os, const MinBoundingSphere &sphere) {
        os << "center: " << boost::geometry::wkt(sphere.center) << " radius: " << sphere.radius
           << " squared_radius: "
           << sphere.squared_radius;
        return os;
    }
};

#endif //TRAJECTORY_DETECTION_MINBOUNDINGSPHERE_H
