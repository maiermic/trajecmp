#ifndef TRAJECMP_GESTURE_CIRCLE_HPP
#define TRAJECMP_GESTURE_CIRCLE_HPP

#include <trajecmp/distance/distances_to_start.hpp>
#include <trajecmp/distance/modified_hausdorff.hpp>
#include <trajecmp/geometry/hyper_sphere.hpp>
#include <trajecmp/util/angle.hpp>
#include <trajecmp/util/approx.hpp>
#include <trajecmp/util/find_local_extrema.hpp>
#include <trajecmp/model/point.hpp>
#include <trajecmp/trait/number_type_trait.hpp>
#include <boost/geometry.hpp>

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
        Angle start_angle;
        Angle end_angle;
        Point center;
        Radius radius;
        int winding_number;
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
        const auto winding_number =
                (extrema.minima.size() < extrema.maxima.size() &&
                 (is_clockwise_winding_direction
                  ? end_angle_clockwise <= std::fmod(start_angle_clockwise + r_45, r_360)
                  : end_angle_counterclockwise <= std::fmod(start_angle_counterclockwise + r_45, r_360)))
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
                    };
                } else {
                    // s < w < w2
                    // 0 -> 45 -> 90
                    // clockwise
                    return {
                            start_angle_clockwise,
                            end_angle_clockwise + winding_angle,
                            center,
                            radius,
                            winding_number,
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
                };
            } else {
                if (w2_angle_clockwise < w_angle_clockwise) {
                    // w2 < w < s
                    // 180 -> 90 -> 0
                    // 330 -> 240 -> 150
                    // counterclockwise
                    return {
                            -start_angle_counterclockwise,
                            -end_angle_counterclockwise - winding_angle,
                            center,
                            radius,
                            winding_number,
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
                    };
                }
            }
        }
    }

}} // namespace trajecmp::util


#endif //TRAJECMP_GESTURE_CIRCLE_HPP
