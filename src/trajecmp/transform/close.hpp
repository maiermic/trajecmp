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

    /**
     * Closes trajectory if distance of start and end point is less than a
     * max_distance.
     *
     * @tparam Trajectory
     * @tparam Distance
     * @param max_distance
     * @param trajectory
     * @return <code>true</code> if trajectory got closed.
     */
    template<
            typename Trajectory,
            typename Distance = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    bool
    close_with_max_distance(Distance max_distance, Trajectory &trajectory) {
        const auto first = trajectory.front();
        const auto last = trajectory.back();
        if (boost::geometry::distance(first, last) > max_distance) {
            return false;
        }
        boost::geometry::append(trajectory, first);
        return true;
    }

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_CLOSE_HPP
