#ifndef TRAJECMP_DISTANCE_DISTANCES_TO_POINT_HPP
#define TRAJECMP_DISTANCE_DISTANCES_TO_POINT_HPP

#include <algorithm>
#include <iterator>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/distance.hpp>

namespace trajecmp { namespace distance {

    /**
     * Calculates the distance of each point of a trajectory to a specific point.
     * <code>boost::geometry::distance</code> is used as distance function.
     */
    template<
            typename Trajectory,
            typename Point = typename boost::geometry::point_type<Trajectory>::type,
            typename DistanceType = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Distances = std::vector<DistanceType>
    >
    Distances distances_to_point(const Point & point,
                                 const Trajectory &trajectory) {
        Distances distances;
        std::transform(
                std::begin(trajectory),
                std::end(trajectory),
                std::back_inserter(distances),
                [&](const Point &current) {
                    return boost::geometry::distance(point, current);
                }
        );
        return distances;
    }

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_DISTANCES_TO_POINT_HPP
