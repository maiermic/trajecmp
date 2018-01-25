#ifndef TRAJECMP_DISTANCE_AVERAGE_DISTANCE_TO_POINT_HPP
#define TRAJECMP_DISTANCE_AVERAGE_DISTANCE_TO_POINT_HPP

#include <algorithm>
#include <iterator>

#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include "distances_to_point.hpp"

namespace trajecmp { namespace distance {

    /**
     * Calculates the average distance of all points of a trajectory to a
     * specific point.
     * <code>boost::geometry::distance</code> is used as distance function.
     */
    template<
            typename Trajectory,
            typename Point = typename boost::geometry::point_type<Trajectory>::type
    >
    typename boost::geometry::coordinate_type<Trajectory>::type
    average_distance_to_point(const Point &point,
                              const Trajectory &trajectory) {
        using Float = typename boost::geometry::coordinate_type<Trajectory>::type;

        const auto radii = distances_to_point(point, trajectory);
        const auto sum = std::accumulate(std::begin(radii), std::end(radii),
                                         Float(0));
        return sum / radii.size();
    }

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_AVERAGE_DISTANCE_TO_POINT_HPP
