#ifndef TRAJECTORY_DETECTION_DISTANCE_H
#define TRAJECTORY_DETECTION_DISTANCE_H

namespace distance {
    template <class Trajectory>
    double modified_hausdorff(const Trajectory &t1, const Trajectory &t2) {
        return boost::geometry::distance(t1, t2);
    };
} // namespace distance

#endif //TRAJECTORY_DETECTION_DISTANCE_H
