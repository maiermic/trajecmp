#ifndef TRAJECMP_DISTANCE_DISTANCE_INFO_HPP
#define TRAJECMP_DISTANCE_DISTANCE_INFO_HPP

#include <algorithm>
#include <iterator>

#include <boost/algorithm/clamp.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/extensions/algorithms/distance_info.hpp>
#include <trajecmp/util/approx.hpp>

namespace trajecmp { namespace distance {

    /**
     * Simliar to <code>boost::geometry::distance_info</code> but with fixed
     * fraction and fixed projected points.
     *
     *
     * See https://gitter.im/boostorg/geometry?at=59e3878af7299e8f53ee02a7
     * See https://gitter.im/boostorg/geometry?at=59e3a246177fb9fe7e8063f6
     */
    template<typename Point, typename Trajectory>
    void distance_info(const Point &point,
                       const Trajectory &trajectory,
                       boost::geometry::distance_info_result<Point> &result) {
        boost::geometry::distance_info(point, trajectory, result);
        auto fraction2 = result.fraction1;
        result.projected_point2 = point;
        // Only fraction1 and projected_point1 are calculated.
        // They are always related to the linestring (trajectory) even if the
        // linestring is passed second.
        auto fraction_of_trajectory =
                boost::algorithm::clamp(result.fraction1, 0, 1);
        auto projected_point_of_trajectory = result.projected_point1;
        if (result.fraction1 < 0 || 1 < result.fraction1) {
            // bug to fix: projected point is outside of segment
            // bug: segment index is not set => check segment points
            const trajecmp::util::approx d(result.real_distance);
            for (const Point &trajectory_point : trajectory) {
                if (boost::geometry::distance(point, trajectory_point) == d) {
                    projected_point_of_trajectory = trajectory_point;
                    break;
                }
            }
        }
        result.fraction1 = 0;
        result.projected_point1 = point;
        result.fraction2 = fraction_of_trajectory;
        result.projected_point2 = projected_point_of_trajectory;
    }

}} // namespace trajecmp::distance

#endif //TRAJECMP_DISTANCE_DISTANCE_INFO_HPP
