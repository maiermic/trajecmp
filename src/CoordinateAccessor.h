#ifndef TRAJECTORY_DETECTION_COORDINATEACCESSOR_H
#define TRAJECTORY_DETECTION_COORDINATEACCESSOR_H

template <typename Geometry>
struct CoordinateAccessor {
    using Pit = typename Geometry::const_iterator;
    using Cit = CoordinateIterator<typename bg::point_type<Geometry>::type>;
    inline Cit operator() (Pit it) const { return Cit(*it); }
};

#endif //TRAJECTORY_DETECTION_COORDINATEACCESSOR_H
