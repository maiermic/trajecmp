#ifndef TRAJECMP_TRANSFORM_SUB_TRAJECTORY_HPP
#define TRAJECMP_TRANSFORM_SUB_TRAJECTORY_HPP

#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/is_empty.hpp>
#include <boost/geometry/algorithms/length.hpp>
#include <boost/geometry/algorithms/num_points.hpp>

namespace trajecmp { namespace transform {

    template<class Point>
    Point point_at_percentage_between(const Point &start,
                                      const Point &end,
                                      const double percentage) {
        namespace bg = boost::geometry;
        Point point_between = end;
        bg::subtract_point(point_between, start);
        bg::multiply_value(point_between, percentage);
        bg::add_point(point_between, start);
        return point_between;
    }

    template<class Trajectory>
    auto sub_trajectory(const Trajectory &trajectory,
                        const double percentage_begin,
                        const double percentage_end) {
        namespace bg = boost::geometry;
        if (bg::num_points(trajectory) <= 1) {
            return Trajectory(trajectory);
        }
        const double length_total = bg::length(trajectory);
        const double length_begin = length_total * percentage_begin;
        const double length_end = length_total * percentage_end;
        double current_length = 0.0;
        auto previous_point = trajectory.front();
        Trajectory neighbours;
        for (auto current_point : trajectory) {
            const auto previous_length = current_length;
            const auto segment_length = bg::distance(previous_point,
                                                     current_point);
            current_length += segment_length;
            if (current_length >= length_begin) {
                if (bg::is_empty(neighbours)) {
                    const auto segment_to_begin_percentage =
                            (0.0 == segment_length)
                            ? 0.0
                            : (length_begin - previous_length) / segment_length;
                    auto first_neighbour_point =
                            point_at_percentage_between(previous_point,
                                                        current_point,
                                                        segment_to_begin_percentage);
                    bg::append(neighbours, first_neighbour_point);
                }
                if (current_length >= length_end) {
                    const auto segment_to_end_percentage =
                            (0.0 == segment_length)
                            ? 0.0
                            : (length_end - previous_length) / segment_length;
                    auto last_neighbour_point =
                            point_at_percentage_between(previous_point,
                                                        current_point,
                                                        segment_to_end_percentage);
                    bg::append(neighbours, last_neighbour_point);
                    break;
                }
                if (current_length > length_begin) {
                    bg::append(neighbours, current_point);
                }
            }
            previous_point = current_point;
        }
        return neighbours;
    }

}} // namespace trajecmp::transform

#endif //TRAJECMP_TRANSFORM_SUB_TRAJECTORY_HPP
