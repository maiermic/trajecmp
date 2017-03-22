#ifndef TRAJECTORY_DETECTION_DISTANCE_H
#define TRAJECTORY_DETECTION_DISTANCE_H

#include <boost/geometry.hpp>

namespace distance {
    namespace detail {
        template<class Trajectory>
        double modified_hausdorff(const Trajectory &t1, const Trajectory &t2) {
            const auto begin = std::begin(t1);
            const auto length_t1 = boost::geometry::length(t1);
            double max = 0.0;
            for (auto && current : t1) {
                const auto current_distance =
                        boost::geometry::distance(current, t2);
                max = std::max(max, current_distance);
            }
            return max;
        };
    } // namespace detail

    template<class Trajectory>
    double modified_hausdorff(const Trajectory &t1, const Trajectory &t2) {
        return std::max(
                detail::modified_hausdorff(t1, t2),
                detail::modified_hausdorff(t2, t1)
        );
    };
} // namespace distance

#endif //TRAJECTORY_DETECTION_DISTANCE_H
