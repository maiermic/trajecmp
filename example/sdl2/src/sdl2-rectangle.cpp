#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <pattern_matching.hpp>
#include <sdl2_framework.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/extensions/strategies/cartesian/distance_info.hpp>
#include <trajecmp/transform/close.hpp>
#include <trajecmp/trajectory/rectangle.hpp>
#include "trajecmp/util/angle.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/vector_ostream.hpp"
#include "trajecmp/gesture/rectangle.hpp"
#include "trajecmp/transform/douglas_peucker.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"

template<
        class Trajectory,
        class Point = typename boost::geometry::point_type<Trajectory>::type,
        class Index = typename Trajectory::difference_type
>
struct rectangle_comparison_data {
    Trajectory preprocessed_input_trajectory;
    Trajectory preprocessed_pattern_trajectory;
    boost::geometry::distance_info_result<Point> distance;
    trajecmp::gesture::rectangle_corner_indices<Index> preprocessed_input_corner_indices;
    boost::geometry::model::box<Point> preprocessed_pattern_corners;
};

template<
        class Trajectory,
        class Point = typename boost::geometry::point_type<Trajectory>::type,
        class Index = typename Trajectory::difference_type
>
rectangle_comparison_data<Trajectory, Point, Index>
get_rectangle_comparison_data(const Trajectory &input_trajectory) {
    namespace bg = boost::geometry;
    using box = bg::model::box<Point>;
    const box min_bounding_rectangle =
            bg::return_envelope<box>(input_trajectory);
    using trajecmp::gesture::estimate_rectangle_corners;
    const auto corners = estimate_rectangle_corners(min_bounding_rectangle,
                                                    input_trajectory);
    const Trajectory preprocessed_input_trajectory =
            trajecmp::gesture::get_normalized_rectangle_trajectory(
                    corners, input_trajectory);
    const Trajectory preprocessed_pattern_trajectory =
            trajecmp::trajectory::rectangle<Trajectory>(min_bounding_rectangle);
    return {
            preprocessed_input_trajectory,
            preprocessed_pattern_trajectory,
            pattern_matching::modified_hausdorff_info(
                    preprocessed_input_trajectory,
                    preprocessed_pattern_trajectory
            ),
            estimate_rectangle_corners(min_bounding_rectangle,
                                       preprocessed_input_trajectory),
            min_bounding_rectangle,
    };
}

struct framework : public record_trajectory_sdl2_framework {

    void handle_input_trajectory(model::trajectory input) {
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::transform::translate_by;
        using trajecmp::geometry::negative_vector_of;
        using trajecmp::transform::scale_to_const;

        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 5) return;
        const auto mbs = trajecmp::geometry::min_bounding_sphere(input);
        input = translate_by(negative_vector_of(mbs.center))(input);
        input = scale_to_const<pm::normalized_size>(mbs.radius * 2)(input);
        bg::append(input, *std::begin(input));
        draw_rectangle_comparison_data(get_rectangle_comparison_data(input));
    }

    void draw_rectangle_comparison_data(
            const rectangle_comparison_data<model::trajectory> &data) {
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;
        const model::trajectory &input_trajectory =
                data.preprocessed_input_trajectory;
        const model::trajectory &pattern_trajectory =
                data.preprocessed_pattern_trajectory;
        const bg::distance_info_result <model::point> &distance = data.distance;
        static const auto visualization_size = 300;
        int w, h;
        SDL_GetRendererOutputSize(_renderer, &w, &h);
        const int center_x = w / 2;
        const int center_y = h / 2;
        const model::vector center(center_x, center_y);
        const auto transform_for_visualization = trajecmp::functional::pipe(
                trajecmp::transform::scale_to_const<visualization_size>(
                        pm::normalized_size),
                trajecmp::transform::translate_by(center)
        );
        const auto is_similar = distance.real_distance <
                                pattern_matching::normalized_size *
                                0.20;

        draw_trajectory(_renderer, transform_for_visualization(
                pattern_trajectory), color_code::yellow);
        const model::trajectory visualization_input_trajectory =
                transform_for_visualization(input_trajectory);
        draw_trajectory(_renderer,
                        visualization_input_trajectory,
                        is_similar ? color_code::green
                                   : color_code::red);
        model::trajectory distance_trajectory{
                distance.projected_point1,
                distance.projected_point2,
        };
        draw_trajectory(_renderer, transform_for_visualization(
                distance_trajectory), color_code::pink);
        draw_box(_renderer, center, 10, color_code::gray);
        LOG(distance.real_distance);

        LOG(data.preprocessed_input_corner_indices.min_corner);
        LOG(data.preprocessed_input_corner_indices.max_corner);
        draw_box(_renderer,
                 visualization_input_trajectory.at(
                         data.preprocessed_input_corner_indices.min_corner),
                 10,
                 color_code::pink);
        draw_box(_renderer,
                 visualization_input_trajectory.at(
                         data.preprocessed_input_corner_indices.max_corner),
                 10,
                 color_code::cyan);

        SDL_RenderPresent(_renderer);
        is_rerender(false);
    }

};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}