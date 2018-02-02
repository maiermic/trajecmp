#ifndef TRAJECMP_TRANSFORM_TRIANGLE_HPP
#define TRAJECMP_TRANSFORM_TRIANGLE_HPP

#include <array>
#include <algorithm>
#include <type_traits>

#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/geometries/box.hpp>

#include "range/v3/at.hpp"
#include "range/v3/view/concat.hpp"
#include "range/v3/view/take.hpp"
#include "range/v3/view/transform.hpp"
#include "range/v3/view/drop.hpp"
#include "range/v3/view/reverse.hpp"
#include "range/v3/view/single.hpp"
#include "range/v3/to_container.hpp"

#include <trajecmp/algorithm/minmax_index.hpp>
#include <trajecmp/algorithm/find_index.hpp>
#include <trajecmp/distance/distances_to_point.hpp>
#include <trajecmp/range/to_trajectory.hpp>
#include <ostream>
#include <boost/geometry/extensions/strategies/cartesian/distance_info.hpp>

namespace trajecmp { namespace transform {

    template<class Index>
    using corner_indices_type = std::vector<Index>;

    /**
     * Map each point of the pattern to the index of the nearest point of
     * the input trajectory.
     *
     * @tparam Trajectory
     * @tparam Point
     * @tparam Index
     * @param pattern
     * @param input
     * @return
     */
    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type,
            class Index = typename Trajectory::difference_type
    >
    corner_indices_type<Index> estimate_corners(
            const Trajectory &pattern,
            const Trajectory &input) {
        return pattern |
               ranges::view::transform([&](const Point &p) {
                   using trajecmp::algorithm::min_index;
                   using trajecmp::distance::distances_to_point;
                   return min_index(distances_to_point(p, input));
               }) |
               ranges::to_vector;
    }

    /**
     * Get the next higher index to the first index of a closed trajectory.
     *
     * @tparam Index
     * @param corner_indices Corner indices of a closed trajectory.
     * @return The index of the second corner.
     */
    template<class Index>
    Index get_second_corner_index(
            const corner_indices_type<Index> &corner_indices) {
        corner_indices_type<Index> sorted_corner_indices = corner_indices;
        sorted_corner_indices.pop_back(); // remove closing point index (duplicate)
        std::sort(std::begin(sorted_corner_indices),
                  std::end(sorted_corner_indices));
        const Index sorted_min_corner_index =
                trajecmp::algorithm::find_index(sorted_corner_indices,
                                                corner_indices.front());
        return sorted_corner_indices.at(
                (1 + sorted_min_corner_index) % (corner_indices.size() - 1));
    }

    /**
     * Check the indices to see if the trajectory has been reversed.
     * @tparam Index 
     * @param corner_indices Indices of trajectory corners.
     * @return <code>true</code> if trajectory is reversed.
     */
    template<class Index>
    bool is_reversed_corner_index_order(
            const corner_indices_type<Index> &corner_indices) {
        const Index second_corner_index =
                get_second_corner_index(corner_indices);
        return second_corner_index == corner_indices.at(1);
    }

    /**
     * Rearrange the corners of the input trajectory according to the corner
     * indices.
     *
     * @tparam Trajectory
     * @tparam Point
     * @tparam Index
     * @param corner_indices
     * @param input_trajectory
     * @return Input trajectory with point order of the corner indices.
     */
    template<
            class Trajectory,
            class Point = typename boost::geometry::point_type<Trajectory>::type,
            class Index = typename Trajectory::difference_type
    >
    Trajectory rearrange_corners_according_to_indices(
            const corner_indices_type<Index> &corner_indices,
            const Trajectory &input_trajectory) {
        using ranges::view::concat;
        using ranges::view::drop;
        using ranges::view::reverse;
        using ranges::view::take;

        const auto first_corner_index = corner_indices.front();
        const trajecmp::range::to_trajectory<Trajectory> to_trajectory{};
        const auto first_corner = ranges::view::single(
                ranges::at(input_trajectory, first_corner_index));
        const auto head =
                input_trajectory | take(first_corner_index) | drop(1);
        const auto tail = input_trajectory | drop(first_corner_index);
        return is_reversed_corner_index_order(corner_indices)
               ? first_corner_index == 0
                 ? input_trajectory
                 : to_trajectory(concat(tail, head, first_corner))
               : first_corner_index == 0
                 ? to_trajectory(reverse(input_trajectory))
                 : to_trajectory(concat(first_corner, reverse(head),
                                        reverse(tail)));
    }

    /**
     * Rearranges the corners of the input in the same order as the pattern.
     * Input and pattern trajectory must be closed.
     *
     * @tparam Trajectory
     * @param pattern_trajectory Closed pattern trajectory.
     * @param input_trajectory Closed input trajectory.
     * @return Input trajectory with the same corner order as the pattern.
     */
    template<class Trajectory>
    Trajectory rearrange_closed_input_using_pattern_corners(
            const Trajectory &pattern_trajectory,
            const Trajectory &input_trajectory) {
        return rearrange_corners_according_to_indices(
                estimate_corners(pattern_trajectory, input_trajectory),
                input_trajectory);
    }

}} // namespace trajecmp::transform


#endif //TRAJECMP_TRANSFORM_TRIANGLE_HPP
