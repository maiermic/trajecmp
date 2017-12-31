#ifndef TRAJECMP_TRANSFORM_CLOSE_HPP
#define TRAJECMP_TRANSFORM_CLOSE_HPP

#include <type_traits>

namespace trajecmp { namespace transform {

    // TODO close_with_max_distance
    // Get tail sub-trajectory T of input trajectory I with length of
    // max-distance (part that might be cut).
    // Get point P of T that has min-distance to start point S of I.
    // Cut part after P and connect it to S to get the closed trajectory.

    struct close {
        template<typename Trajectory>
        Trajectory operator()(Trajectory trajectory) {
            boost::geometry::append(trajectory, *std::begin(trajectory));
            return trajectory;
        }
    };

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_CLOSE_HPP
