#ifndef TRAJECMP_GESTURE_CIRCLE_HPP
#define TRAJECMP_GESTURE_CIRCLE_HPP

#include <array>
#include <algorithm>

#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/geometries/box.hpp>

#include "range/v3/at.hpp"
#include "range/v3/view/concat.hpp"
#include "range/v3/view/drop.hpp"
#include "range/v3/view/reverse.hpp"
#include "range/v3/view/single.hpp"
#include "range/v3/view/take.hpp"

#include <trajecmp/algorithm/minmax_index.hpp>
#include <trajecmp/algorithm/find_index.hpp>
#include <trajecmp/distance/distances_to_point.hpp>
#include <trajecmp/range/to_trajectory.hpp>
#include <ostream>

namespace trajecmp { namespace gesture {

    template<class Index>
    struct rectangle_corner_indices {
        using index_type = Index;
        Index min_corner;
        Index min_x_max_y_corner;
        Index max_corner;
        Index max_x_min_y_corner;

        friend std::ostream &
        operator<<(std::ostream &os, const rectangle_corner_indices &indices) {
            os << "{ min_corner: " << indices.min_corner
               << " min_x_max_y_corner: "
               << indices.min_x_max_y_corner << " max_corner: "
               << indices.max_corner << " min_y_max_x_corner: "
               << indices.max_x_min_y_corner << '}';
            return os;
        }

        bool operator==(const rectangle_corner_indices &rhs) const {
            return min_corner == rhs.min_corner &&
                   min_x_max_y_corner == rhs.min_x_max_y_corner &&
                   max_corner == rhs.max_corner &&
                   max_x_min_y_corner == rhs.max_x_min_y_corner;
        }

        bool operator!=(const rectangle_corner_indices &rhs) const {
            return !(rhs == *this);
        }
    };

    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type
    >
    auto estimate_rectangle_corners(
            const boost::geometry::model::box<Point> min_bounding_rectangle,
            const Trajectory &trajectory) {
        namespace bg = boost::geometry;
        using trajecmp::algorithm::minmax_index;
        using trajecmp::distance::distances_to_point;
        assert(bg::num_points(trajectory) >= 1);
        const auto distances_to_min_corner =
                distances_to_point(min_bounding_rectangle.min_corner(),
                                   trajectory);
        const Point min_x_max_y_corner(
                bg::get<0>(min_bounding_rectangle.min_corner()),
                bg::get<1>(min_bounding_rectangle.max_corner()));
        const auto distances_to_min_x_max_y_corner =
                distances_to_point(min_x_max_y_corner, trajectory);
        const auto extrema_of_min_corner =
                minmax_index(distances_to_min_corner);
        const auto extrema_of_min_x_max_y_corner =
                minmax_index(distances_to_min_x_max_y_corner);
        return rectangle_corner_indices<decltype(extrema_of_min_corner.first)> {
                extrema_of_min_corner.first,
                extrema_of_min_x_max_y_corner.first,
                extrema_of_min_corner.second,
                extrema_of_min_x_max_y_corner.second,
        };
    }

    template<class Index>
    std::array<Index, 4> get_sorted_rectangle_corner_indices(
            const rectangle_corner_indices<Index> &corner_indices) {
        std::array<Index, 4> sorted_corner_indices{
                corner_indices.min_corner,
                corner_indices.min_x_max_y_corner,
                corner_indices.max_corner,
                corner_indices.max_x_min_y_corner,
        };
        std::sort(std::begin(sorted_corner_indices),
                  std::end(sorted_corner_indices));
        return sorted_corner_indices;
    }

    template<class Index>
    Index get_corner_after_min_corner(
            const rectangle_corner_indices<Index> &corner_indices) {
        const std::array<Index, 4> sorted_corner_indices =
                get_sorted_rectangle_corner_indices(corner_indices);
        const auto sorted_min_corner_index =
                trajecmp::algorithm::find_index(sorted_corner_indices,
                                                corner_indices.min_corner);
        return sorted_corner_indices.at((1 + sorted_min_corner_index) % 4);
    }

    /**
     * Returns normalized rectangle trajectory with corner order:
     * min_corner, min_x_max_y_corner, max_corner, min_y_max_x_corner.
     * @param corner_indices Indices of the corners of <code>trajectory</code>.
     * @param trajectory The input trajectory that should be normalized.
     * @return Normalized rectangle trajectory.
     */
    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type,
            class Index = typename Trajectory::difference_type
    >
    Trajectory get_normalized_rectangle_trajectory(
            const rectangle_corner_indices<Index> &corner_indices,
            const Trajectory &trajectory) {
        const Index corner_after_min_corner =
                get_corner_after_min_corner(corner_indices);
        const bool is_clockwise =
                corner_after_min_corner == corner_indices.min_x_max_y_corner;
        using ranges::view::concat;
        using ranges::view::drop;
        using ranges::view::reverse;
        using ranges::view::take;
        const trajecmp::range::to_trajectory<Trajectory> to_trajectory{};
        const auto min_corner = ranges::view::single(
                ranges::at(trajectory, corner_indices.min_corner));
        const auto head =
                trajectory | take(corner_indices.min_corner) | drop(1);
        const auto tail = trajectory | drop(corner_indices.min_corner);
        return is_clockwise
               ? corner_indices.min_corner == 0
                 ? trajectory
                 : to_trajectory(concat(tail, head, min_corner))
               : corner_indices.min_corner == 0
                 ? to_trajectory(reverse(trajectory))
                 : to_trajectory(concat(min_corner, reverse(head),
                                        reverse(tail)));
    }

}} // namespace trajecmp::gesture


#endif //TRAJECMP_GESTURE_CIRCLE_HPP
