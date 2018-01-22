#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>
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
#include "trajecmp/distance/distances_to_point.hpp"
#include "trajecmp/util/angle.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/util/find_local_extrema.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/vector_ostream.hpp"
#include "trajecmp/gesture/circle.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/transform/douglas_peucker.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"

struct circle_comparison_data {
    model::trajectory preprocessed_input_trajectory;
    model::trajectory preprocessed_pattern_trajectory;
    boost::geometry::distance_info_result<model::point> distance;
};

circle_comparison_data
get_circle_comparison_data(const model::trajectory &input_trajectory) {
    using trajecmp::util::r2d;
    namespace pm = pattern_matching;
    const auto mbs = trajecmp::geometry::min_bounding_sphere(input_trajectory);
    const auto c = trajecmp::gesture::estimate_circle_segment(input_trajectory, mbs);
    LOG(r2d(c.start_angle));
    LOG(r2d(c.end_angle));
    LOG(c.winding_number);
    LOG_SEP();
    const auto radii =
        trajecmp::distance::distances_to_point(mbs.center, input_trajectory);
    const auto sum = std::accumulate(radii.begin(),
                                     radii.end(), 0.0f);
    const auto radius_factor = (sum / radii.size()) / mbs.radius;
    const auto pattern_trajectory =
            trajecmp::trajectory::circle<model::trajectory>(radius_factor * pm::normalized_size / 2)
                    .sample(r2d(c.start_angle), r2d(c.end_angle), 5.0f);
    const auto preprocess_input = [&](model::trajectory trajectory) {
        return trajecmp::transform::scale_to_const<pm::normalized_size>(mbs.radius * 2)(
                trajecmp::transform::translate_by(trajecmp::geometry::negative_vector_of(c.center))(
                        trajectory)
        );
    };
    const model::trajectory preprocessed_input_trajectory =
            preprocess_input(input_trajectory);
    return {
            preprocessed_input_trajectory,
            pattern_trajectory,
            pm::modified_hausdorff_info(
                    preprocessed_input_trajectory,
                    pattern_trajectory
            ),
    };
}

class framework : public record_trajectory_sdl2_framework {

    void handle_input_trajectory(model::trajectory input) {
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;

        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 3) return;
        auto data = get_circle_comparison_data(input);
        draw_comparison_data(data);
    }

    void draw_comparison_data(circle_comparison_data &data) {
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;
        model::trajectory &input_trajectory =
                data.preprocessed_input_trajectory;
        model::trajectory &pattern_trajectory =
                data.preprocessed_pattern_trajectory;
        bg::distance_info_result <model::point> &distance =
                data.distance;
        const auto is_similar = distance.real_distance <
                                pattern_matching::normalized_size *
                                0.20;
        model::trajectory distance_trajectory{
                distance.projected_point1,
                distance.projected_point2,
        };
        transform_for_visualization(pattern_trajectory);
        transform_for_visualization(input_trajectory);
        transform_for_visualization(distance_trajectory);
        draw_trajectory(_renderer, pattern_trajectory, color_code::yellow);
        draw_trajectory(_renderer, input_trajectory,
                        is_similar ? color_code::green
                                   : color_code::red);

        draw_trajectory(_renderer, distance_trajectory, color_code::pink);
        draw_box(_renderer, get_visualization_center(), 10, color_code::gray);
        LOG(distance.real_distance);

        SDL_RenderPresent(_renderer);
        is_rerender(false);
    }

};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}