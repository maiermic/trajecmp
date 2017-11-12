#ifndef TRAJECMP_GESTURE_CIRCLE_HPP
#define TRAJECMP_GESTURE_CIRCLE_HPP

#include <trajecmp/distance/distances_to_start.hpp>
#include <trajecmp/geometry/hyper_sphere.hpp>
#include <trajecmp/util/angle.hpp>
#include <trajecmp/util/find_local_extrema.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/trait/number_type_trait.hpp>
#include <boost/geometry.hpp>

namespace trajecmp { namespace gesture {

    template <typename Angle>
    struct circle_segment_info {
        Angle start_angle;
        Angle end_angle;
    };

    /**
     * Estimate the circle segment that a trajectory might represent.
     * The trajectory might not be a circle segment.
     * However, you can create a circle trajectory based on the result of this
     * function to compare it with the trajectory to check if it is.
     */
    template <
            typename Trajectory,
            typename Point = typename boost::geometry::point_type<Trajectory>::type,
            typename Angle = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    circle_segment_info<Angle>
    estimate_circle_segment(const Trajectory &trajectory,
                            const trajecmp::geometry::hyper_sphere_of<Trajectory> &min_bounding_sphere) {
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        using number_type_trait = trajecmp::trait::number_type_trait<Coordinate>;
        using trajecmp::util::angle_clockwise;
        using trajecmp::util::angle_counterclockwise;
        using trajecmp::util::r2d;
        using trajecmp::util::d2r;
        using trajecmp::model::operator-;
        const Point x_achsis(
                number_type_trait::get_one_element(),
                number_type_trait::get_zero_element()
        );

        const auto distances_to_start =
                trajecmp::distance::distances_to_start(trajectory);
        const auto extrema =
                trajecmp::util::find_local_extrema(distances_to_start, min_bounding_sphere.radius / 10);

        const auto winding_direction_reference_point_index =
                extrema.maxima.size() > 0
                ? extrema.maxima.front() / 2
                : trajectory.size() / 2;
        const auto winding_direction_reference_point =
                trajectory[winding_direction_reference_point_index];
        const Angle winding_direction_reference_point_angle = angle_clockwise(
                x_achsis,
                winding_direction_reference_point - min_bounding_sphere.center
        );

        const bool is_clockwise_winding_direction =
                winding_direction_reference_point_angle < d2r(Angle(180.0));
        const int winding_direction = is_clockwise_winding_direction ? 1 : -1;
        const auto angle =
                is_clockwise_winding_direction
                    ? angle_clockwise<Point>
                    : angle_counterclockwise<Point>;

        const Angle start_angle = angle(
                x_achsis,
                trajectory.front() - min_bounding_sphere.center
        );
        const Angle end_angle = angle(
                x_achsis,
                trajectory.back() - min_bounding_sphere.center
        );
        int winding_number =
                (extrema.maxima.size() + extrema.minima.size()) / 2;
        static const Angle eps = number_type_trait::get_default_eps();
        if (extrema.minima.size() < extrema.maxima.size() && end_angle < eps) {
            ++winding_number;
        }
        return {
                start_angle,
                end_angle * winding_direction + d2r(Angle(360 * winding_number * winding_direction)),
        };
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_GESTURE_CIRCLE_HPP