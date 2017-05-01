#ifndef TRAJECMP_TRANSFORM_DOUGLAS_PEUCKER_HPP
#define TRAJECMP_TRANSFORM_DOUGLAS_PEUCKER_HPP

namespace trajecmp { namespace transform {

    template<typename Distance>
    auto douglas_peucker(const Distance &max_distance) {
        return [=](auto &&trajectory) {
            using Trajectory = typename std::remove_reference<decltype(trajectory)>::type;
            Trajectory simplified;
            boost::geometry::simplify(trajectory,
                                      simplified,
                                      max_distance);
            return simplified;
        };
    }

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_DOUGLAS_PEUCKER_HPP
