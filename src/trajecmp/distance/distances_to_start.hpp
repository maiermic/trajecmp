#ifndef TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP
#define TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP

#include <algorithm>
#include <iterator>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/distance.hpp>

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
        using Point = typename boost::geometry::point_type<Trajectory>::type;
        Distances distances;
        const Point start =
                trajectory.front();
        std::transform(
                std::begin(trajectory),
                std::end(trajectory),
                std::back_inserter(distances),
                [&](const Point &current) {
                    return boost::geometry::distance(start, current);
                }
        );
        return distances;
    }

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP
