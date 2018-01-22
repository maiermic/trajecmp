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
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/extensions/strategies/cartesian/distance_info.hpp>
#include <trajecmp/gesture/rectangle.hpp>
#include <trajecmp/gesture/circle.hpp>
#include "trajecmp/geometry/hyper_sphere.hpp"
#include "trajecmp/util/angle.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/util/find_local_extrema.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/vector_ostream.hpp"
#include "trajecmp/gesture/circle.hpp"
#include "trajecmp/gesture/rectangle.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/transform/douglas_peucker.hpp"
#include "trajecmp/transform/translate_and_scale.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"

struct circle_comparison_data {
    model::trajectory preprocessed_input_trajectory;
    model::trajectory preprocessed_pattern_trajectory;
    boost::geometry::distance_info_result<model::point> distance;
};

circle_comparison_data
get_circle_comparison_data(
        const model::trajectory &input_trajectory,
        const trajecmp::geometry::hyper_sphere_of<model::trajectory> &mbs) {
    using trajecmp::util::r2d;
    using trajecmp::gesture::estimate_circle_trajectory_average_radius_factor_sized;
    namespace pm = pattern_matching;

    const auto pattern_trajectory =
            estimate_circle_trajectory_average_radius_factor_sized(
                    pm::normalized_size, input_trajectory, mbs);
    return {
            input_trajectory,
            pattern_trajectory,
            pm::modified_hausdorff_info(
                    input_trajectory,
                    pattern_trajectory
            ),
    };
}

model::trajectory get_distance_trajectory(
        const boost::geometry::distance_info_result<model::point> &distance) {
    return {
        distance.projected_point1,
        distance.projected_point2,
    };
}

class framework : public record_trajectory_sdl2_framework {

    void handle_input_trajectory(model::trajectory input) override {
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::transform::translate_and_scale_using_mbs;
        using trajecmp::gesture::get_rectangle_comparison_data;
        using pattern_matching::normalized_size;

        // shared preprocessing of input trajectory
        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 3) return;
        auto mbs = trajecmp::geometry::min_bounding_sphere(input);
        translate_and_scale_using_mbs(normalized_size, mbs, input);

        auto circle_data =
                get_circle_comparison_data(input, mbs);
        bg::append(input, *std::begin(input));
        auto rectangle_data = get_rectangle_comparison_data(
                input, pattern_matching::modified_hausdorff_info);

        const bool is_rectangle = rectangle_data.distance.real_distance <
                                  normalized_size * 0.20;
        const bool is_circle = circle_data.distance.real_distance <
                                  normalized_size * 0.20;

        LOG(circle_data.distance.real_distance);
        LOG(rectangle_data.distance.real_distance);
        LOG_SEP();
        if (is_rectangle && is_circle) {
            if (rectangle_data.distance.real_distance <
                circle_data.distance.real_distance) {
                draw_result_data(rectangle_data);
            } else {
                draw_result_data(circle_data);
            }
        } else if (is_rectangle) {
            draw_result_data(rectangle_data);
        } else if (is_circle) {
            draw_result_data(circle_data);
        }
    }

    template <typename Data>
    void draw_result_data(Data &data) {
        draw_trajectories(data.preprocessed_pattern_trajectory,
                          data.preprocessed_input_trajectory,
                          data.distance);
    }

    void draw_trajectories(
            model::trajectory &pattern_trajectory,
            model::trajectory &input_trajectory,
            const boost::geometry::distance_info_result<model::point> &distance) {
        transform_for_visualization(pattern_trajectory);
        transform_for_visualization(input_trajectory);
        model::trajectory distance_trajectory =
                get_distance_trajectory(distance);
        transform_for_visualization(distance_trajectory);
        draw_trajectory(_renderer, pattern_trajectory, color_code::yellow);
        draw_trajectory(_renderer, input_trajectory, color_code::green);
        draw_trajectory(_renderer, distance_trajectory, color_code::pink);
        SDL_RenderPresent(_renderer);
        is_rerender(false);
    }

};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}