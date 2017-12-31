#ifndef TRAJECMP_DISTANCE_DISTANCE_TO_START_HPP
#define TRAJECMP_DISTANCE_DISTANCE_TO_START_HPP

#include <algorithm>
#include <iterator>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include "distances_to_point.hpp"

namespace trajecmp { namespace distance {

    /**
     * Calculates the distance of each point to the first point of a trajectory.
     * <code>boost::geometry::distance</code> is used as distance function.
     */
    template<
            typename Trajectory,
            typename DistanceType = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Distances = std::vector<DistanceType>
    >
    Distances distances_to_start(const Trajectory &trajectory) {
        return trajecmp::distance::distances_to_point(trajectory.front(),
                                                      trajectory);
    }

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_DISTANCE_TO_START_HPP
