#ifndef TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP
#define TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP

#include <algorithm>

#include <boost/geometry.hpp>
#include <boost/geometry/extensions/algorithms/distance_info.hpp>

#include <trajecmp/transform/sub_trajectory.hpp>

namespace trajecmp { namespace distance {
    namespace detail {

        template<class Trajectory, class Neighbours>
        double modified_hausdorff(const Trajectory &t1, const Trajectory &t2, Neighbours neighbours) {
            namespace bg = boost::geometry;
            const auto length_t1 = bg::length(t1);
            double distance = 0.0;
            Trajectory current_trajectory;
            for (auto &&current : t1) {
                bg::append(current_trajectory, current);
                const auto current_percentage =
                        0 == bg::length(current_trajectory)
                        ? 0
                        : bg::length(current_trajectory) / length_t1;
                const auto neighbours_trajectory = neighbours(t2, current_percentage);
                const auto current_distance = bg::distance(current, neighbours_trajectory);
                distance = std::max(distance, current_distance);
            }
            return distance;
        };

        template<class Trajectory, class Neighbours>
        auto modified_hausdorff_info(const Trajectory &t1, const Trajectory &t2, Neighbours neighbours) {
            namespace bg = boost::geometry;
            using point = typename bg::point_type<Trajectory>::type;
            using distance_info_result = bg::distance_info_result<point>;
            const auto length_t1 = bg::length(t1);
            distance_info_result result;
            Trajectory current_trajectory;
            for (auto &&current : t1) {
                bg::append(current_trajectory, current);
                const auto current_percentage =
                        0 == bg::length(current_trajectory)
                        ? 0
                        : bg::length(current_trajectory) / length_t1;
                const auto neighbours_trajectory = neighbours(t2, current_percentage);
                distance_info_result current_result;
                bg::distance_info(current, neighbours_trajectory, current_result);
                if (result.real_distance < current_result.real_distance) {
                    result = current_result;
                    // fix distance_info result
                    // https://gitter.im/boostorg/geometry?at=59e3878af7299e8f53ee02a7
                    result.projected_point2 = current;
                    // https://gitter.im/boostorg/geometry?at=59e3a246177fb9fe7e8063f6
                    if (result.fraction1 < 0) {
                        result.fraction1 = 0;
                        result.projected_point1 =
                                *std::begin(neighbours_trajectory);
                    } else if (result.fraction1 > 1) {
                        result.fraction1 = 1;
                        result.projected_point1 =
                                *(--std::end(neighbours_trajectory));
                    }
                }
            }
            return result;
        };
    } // namespace detail


    class neighbours_percentage_range {
        const double _percentage_max_distance_from_position;

    public:
        neighbours_percentage_range(const double percentage_max_distance_from_position)
                : _percentage_max_distance_from_position(percentage_max_distance_from_position) {}

        template<class Trajectory>
        auto operator()(const Trajectory &trajectory, const double percentage_position) {
            const auto percentage_begin =
                    std::max(0.0, percentage_position - _percentage_max_distance_from_position);
            const auto percentage_end =
                    std::min(1.0, percentage_position + _percentage_max_distance_from_position);
            return trajecmp::transform::sub_trajectory(trajectory, percentage_begin, percentage_end);
        }
    };

    template<class Neighbours>
    constexpr auto modified_hausdorff(Neighbours &&neighbours) {
        return [=](const auto &t1, const auto &t2) {
            return std::max(
                    detail::modified_hausdorff(t1, t2, neighbours),
                    detail::modified_hausdorff(t2, t1, neighbours)
            );
        };
    };

    template<class Neighbours>
    constexpr auto modified_hausdorff_info(Neighbours &&neighbours) {
        return [=](const auto &t1, const auto &t2) {
            const auto d1 = detail::modified_hausdorff_info(t1, t2, neighbours);
            const auto d2 = detail::modified_hausdorff_info(t2, t1, neighbours);
            return (d1.real_distance < d2.real_distance) ? d2 : d1;
        };
    };

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_MODIFIED_HAUSDORFF_HPP
