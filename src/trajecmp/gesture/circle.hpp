#ifndef TRAJECMP_GESTURE_CIRCLE_HPP
#define TRAJECMP_GESTURE_CIRCLE_HPP

#include <iterator>
#include <numeric>

#include <trajecmp/distance/distances_to_start.hpp>
#include <trajecmp/distance/distances_to_point.hpp>
#include <trajecmp/distance/modified_hausdorff.hpp>
#include <trajecmp/geometry/hyper_sphere.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>
#include <trajecmp/util/angle.hpp>
#include <trajecmp/util/approx.hpp>
#include <trajecmp/util/find_local_extrema.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/trait/number_type_trait.hpp>
#include <trajecmp/trajectory/circle.hpp>
#include <boost/geometry.hpp>
#include <trajecmp/distance/average_distance_to_point.hpp>

namespace trajecmp { namespace gesture {

    template<typename Point>
    Point estimate_circle_center(const Point &a,
                                 const Point &b,
                                 const Point &c) {
        using Coordinate = typename boost::geometry::coordinate_type<Point>::type;
        using trajecmp::geometry::point::x;
        using trajecmp::geometry::point::y;

        // solution based on: https://stackoverflow.com/a/4103418/1065654
        const Coordinate offset = std::pow(x(b), 2) + std::pow(y(b), 2);
        const Coordinate bc =
                (std::pow(x(a), 2) + std::pow(y(a), 2) - offset) / 2.0;
        const Coordinate cd =
                (offset - std::pow(x(c), 2) - std::pow(y(c), 2)) / 2.0;
        const Coordinate det =
                (x(a) - x(b)) * (y(b) - y(c)) - (x(b) - x(c)) * (y(a) - y(b));

        const Coordinate idet = 1 / det;

        const Coordinate center_x =
                (bc * (y(b) - y(c)) - cd * (y(a) - y(b))) * idet;
        const Coordinate center_y =
                (cd * (x(a) - x(b)) - bc * (x(b) - x(c))) * idet;

        return Point(center_x, center_y);
    }

    template<
            typename Angle,
            typename Point,
            typename Radius = typename boost::geometry::coordinate_type<Point>::type
    >
    struct circle_segment_info {
        using Float = typename boost::geometry::coordinate_type<Point>::type;

        Angle start_angle;
        Angle end_angle;
        Point center;
        Radius radius;
        int winding_number;
        bool is_clockwise_winding_direction;

        int full_circle_count() const {
            return winding_number;
        }

        Float angle_diff() const {
            return std::abs(start_angle - end_angle);
        }

        Float circle_count() const {
            return angle_diff() / trajecmp::util::d2r(Angle(360.0));
        }
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
            typename Angle = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Radius = typename boost::geometry::coordinate_type<Point>::type
    >
    circle_segment_info<Angle, Point>
    estimate_circle_segment(const Trajectory &trajectory,
                            const trajecmp::geometry::hyper_sphere_of<Trajectory> &min_bounding_sphere) {
        using Coordinate = typename boost::geometry::coordinate_type<Trajectory>::type;
        using number_type_trait = trajecmp::trait::number_type_trait<Coordinate>;
        using trajecmp::util::angle_clockwise;
        using trajecmp::util::angle_counterclockwise;
        using trajecmp::util::approx;
        using trajecmp::util::r2d;
        using trajecmp::util::d2r;
        using trajecmp::model::operator-;
        const Angle r_45 = d2r(Angle(45.0));
        const Angle r_180 = d2r(Angle(180.0));
        const Angle r_360 = d2r(Angle(360.0));
        const Point x_achsis(
                number_type_trait::get_one_element(),
                number_type_trait::get_zero_element()
        );
        const auto epsilon_distance = min_bounding_sphere.radius / 10;
        const auto distances_to_start =
                trajecmp::distance::distances_to_start(trajectory);
        const auto extrema =
                trajecmp::util::find_local_extrema(
                        distances_to_start,
                        epsilon_distance
                );
        const Point center =
                extrema.maxima.size() == 0
                ? estimate_circle_center(
                        trajectory.front(),
                        trajectory.at(trajectory.size() / 2),
                        trajectory.back()
                )
                : min_bounding_sphere.center;
        const Angle start_angle_clockwise = angle_clockwise(
                x_achsis,
                trajectory.front() - center
        );
        const Angle start_angle_counterclockwise = angle_counterclockwise(
                x_achsis,
                trajectory.front() - center
        );
        const Angle end_angle_clockwise = angle_clockwise(
                x_achsis,
                trajectory.back() - center
        );
        const Angle end_angle_counterclockwise = angle_counterclockwise(
                x_achsis,
                trajectory.back() - center
        );
        // estimate winding direction using two points w and w2
        const auto w_index =
                extrema.maxima.size() > 0
                ? extrema.maxima.front() / 2
                : trajectory.size() / 2;
        // TODO prevent std::out_of_range exception
        const auto w = trajectory.at(w_index);
        const Angle w_angle_clockwise = angle_clockwise(x_achsis, w - center);
        const auto w2_index =
                extrema.maxima.size() > 0
                ? extrema.maxima.front()
                : trajectory.size() - 1;
        // TODO prevent std::out_of_range exception
        const auto w2 = trajectory.at(w2_index);
        const Angle w2_angle_clockwise = angle_clockwise(x_achsis, w2 - center);
        // TODO share conditions with if-else branches at the end of this function
        const bool is_clockwise_winding_direction =
                start_angle_clockwise < w2_angle_clockwise
                ? start_angle_clockwise < w_angle_clockwise
                  ? w2_angle_clockwise > w_angle_clockwise
                  : false
                : (start_angle_clockwise < w_angle_clockwise ||
                   !(w2_angle_clockwise < w_angle_clockwise));
        // estimate number of complete circles
        const int winding_number_unfixed =
                (extrema.maxima.size() + extrema.minima.size()) / 2;
        const auto angle_between = [=](const Angle lower,
                                       const Angle angle,
                                       Angle upper) {
            upper = std::fmod(upper, r_360);
            if (upper < lower) {
                return lower <= angle || angle <= upper;
            }
            return lower <= angle && angle <= upper;
        };
        const auto winding_number =
                (extrema.minima.size() < extrema.maxima.size() &&
                 (is_clockwise_winding_direction
                  ? angle_between(start_angle_clockwise,
                                  end_angle_clockwise,
                                  start_angle_clockwise + r_45)
                  : angle_between(start_angle_counterclockwise,
                                  end_angle_counterclockwise,
                                  start_angle_counterclockwise + r_45)))
                ? winding_number_unfixed + 1
                : winding_number_unfixed;
        // TODO use multiple points to estimate radius (e.g. average distance to center)
        const Radius radius = boost::geometry::distance(center, trajectory.front());
        // angle of complete windings
        const auto winding_angle = r_360 * winding_number;
        // In each case-branch of the nested if statement is a comment
        // that shows a representation of the relation of the angles
        // using the abbreviations:
        //   s = start_angle_clockwise
        //   w = w_angle_clockwise
        //   w2 = w2_angle_clockwise
        // Further, it contains example of the trajectory flow of the case:
        //   s -> w -> w2
        if (start_angle_clockwise < w2_angle_clockwise) {
            if (start_angle_clockwise < w_angle_clockwise) {
                if (w2_angle_clockwise < w_angle_clockwise) {
                    // s < w2 < w
                    // 0 -> 270 -> 180
                    // counterclockwise
                    return {
                            start_angle_clockwise,
                            -end_angle_counterclockwise - winding_angle,
                            center,
                            radius,
                            winding_number,
                            is_clockwise_winding_direction,
                    };
                } else {
                    // s < w < w2
                    // 0 -> 45 -> 90
                    // clockwise
                    const Angle end_angle = end_angle_clockwise + winding_angle;
                    const Angle start_angle =
                            (start_angle_clockwise < end_angle_clockwise)
                                ? start_angle_clockwise
                                : -start_angle_counterclockwise;
                    return {
                            start_angle,
                            end_angle,
                            center,
                            radius,
                            winding_number,
                            is_clockwise_winding_direction,
                    };
                }
            } else {
                // w < s < w2
                // 90 -> 0 -> 270
                // counterclockwise
                return {
                        start_angle_counterclockwise,
                        -end_angle_counterclockwise - winding_angle,
                        center,
                        radius,
                        winding_number,
                        is_clockwise_winding_direction,
                };
            }
        } else {
            if (start_angle_clockwise < w_angle_clockwise) {
                // w2 < s < w
                // 270 -> 315 -> 0
                // clockwise
                return {
                        -start_angle_counterclockwise,
                        end_angle_clockwise + winding_angle,
                        center,
                        radius,
                        winding_number,
                        is_clockwise_winding_direction,
                };
            } else {
                if (w2_angle_clockwise < w_angle_clockwise) {
                    // w2 < w < s
                    // 180 -> 90 -> 0
                    // 330 -> 240 -> 150
                    // counterclockwise
                    const Angle end_angle =
                            -end_angle_counterclockwise - winding_angle;
                    const Angle start_angle =
                            (start_angle_counterclockwise < end_angle_counterclockwise)
                            ? -start_angle_counterclockwise
                            : start_angle_clockwise;
                    return {
                            start_angle,
                            end_angle,
                            center,
                            radius,
                            winding_number,
                            is_clockwise_winding_direction,
                    };
                } else {
                    // w < w2 < s
                    // 340 -> 0 -> 20
                    // 350 -> 10 -> 30
                    // clockwise
                    return {
                            -start_angle_counterclockwise,
                            end_angle_clockwise + winding_angle,
                            center,
                            radius,
                            winding_number,
                            is_clockwise_winding_direction,
                    };
                }
            }
        }
    }

    template<
            typename Trajectory,
            typename Size = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    Trajectory estimate_circle_trajectory_average_radius_factor_sized(
            Size max_size,
            const Trajectory &input_trajectory,
            const trajecmp::geometry::hyper_sphere_of<Trajectory> &mbs) {
        using trajecmp::util::r2d;
        using trajecmp::distance::average_distance_to_point;
        using trajecmp::trajectory::circle;
        using Float = Size;

        const auto c = estimate_circle_segment(input_trajectory, mbs);
        const auto average_distance =
                average_distance_to_point(mbs.center, input_trajectory);
        const auto radius_factor = average_distance / mbs.radius;
        return circle<Trajectory>(radius_factor * max_size / Float(2))
                .sample(r2d(c.start_angle), r2d(c.end_angle), Float(5));
    }

    template<
            typename Trajectory,
            typename Size = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    Trajectory estimate_circle_trajectory_average_radius_factor_sized(
            Size max_size,
            const Trajectory &input_trajectory) {
        const auto mbs =
                trajecmp::geometry::min_bounding_sphere(input_trajectory);
        return estimate_circle_trajectory_average_radius_factor_sized(
                max_size, input_trajectory, mbs);
    };

    template<
            typename Trajectory,
            typename Size = typename boost::geometry::coordinate_type<Trajectory>::type,
            typename Point = typename boost::geometry::point_type<Trajectory>::type,
            typename Angle = typename boost::geometry::coordinate_type<Trajectory>::type
    >
    Trajectory get_average_radius_factor_sized_circle_trajectory(
            Size max_size,
            const Trajectory &input_trajectory,
            const trajecmp::geometry::hyper_sphere_of<Trajectory> &mbs,
            const circle_segment_info<Angle, Point> &c) {
        using trajecmp::util::r2d;
        using trajecmp::distance::average_distance_to_point;
        using trajecmp::trajectory::circle;
        using Float = Size;

        const auto average_distance =
                average_distance_to_point(mbs.center, input_trajectory);
        const auto radius_factor = average_distance / mbs.radius;
        return circle<Trajectory>(radius_factor * max_size / Float(2))
                .sample(r2d(c.start_angle), r2d(c.end_angle), Float(5));
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_GESTURE_CIRCLE_HPP
