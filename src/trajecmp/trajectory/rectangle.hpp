#ifndef TRAJECMP_TRAJECTORY_RECTANGLE_HPP
#define TRAJECMP_TRAJECTORY_RECTANGLE_HPP

#include <algorithm>
#include <trajecmp/geometry/point/point.hpp>
#include <boost/geometry/geometries/box.hpp>

namespace trajecmp { namespace trajectory {

    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type,
            class Box = typename boost::geometry::model::box<Point>
    >
    Trajectory rectangle(const Box &box) {
        using trajecmp::geometry::point::x;
        using trajecmp::geometry::point::y;
        const Point &min_corner = box.min_corner();
        const Point &max_corner = box.max_corner();
        const Point min_x_max_y_corner(x(min_corner), y(max_corner));
        const Point max_x_min_y_corner(x(max_corner), y(min_corner));
        return {
                min_corner,
                min_x_max_y_corner,
                max_corner,
                max_x_min_y_corner,
                min_corner,
        };

    }

    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type
    >
    Trajectory rectangle(const Point &corner_1, const Point &corner_2) {
        using trajecmp::geometry::point::x;
        using trajecmp::geometry::point::y;
        const Point min_corner(
                std::min(x(corner_1), x(corner_2)),
                std::min(y(corner_1), y(corner_2))
        );
        const Point max_corner(
                std::max(x(corner_1), x(corner_2)),
                std::max(y(corner_1), y(corner_2))
        );
        using boost::geometry::model::box;
        return rectangle(box<Point>(min_corner, max_corner));

    }

}} // namespace trajecmp::trajectory

#endif //TRAJECMP_TRAJECTORY_RECTANGLE_HPP
