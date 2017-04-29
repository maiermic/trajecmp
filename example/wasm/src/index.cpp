#include "emscripten.h"
#include "SDL.h"
#include <vector>
#include <cmath>

#include <boost/geometry.hpp>

#include "color.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "model.hpp"

#include "../../../src/trajecmp/distance/modified_hausdorff.hpp"
#include "../../../src/trajecmp/geometry/min_bounding_sphere.hpp"
#include "../../../src/trajecmp/util/boost_geometry_to_string.hpp"

#include "../../logging.hpp"
#include "../../TrajectorySvg.hpp"

namespace bg = boost::geometry;
namespace trans = boost::geometry::strategy::transform;

using trajecmp::geometry::min_bounding_sphere;


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_bool done = SDL_FALSE;


auto x_angle(const model::point p) {
    return std::atan2(bg::get<0>(p),
                      bg::get<1>(p));
}

void compare_trajectories(const model::trajectory &input_trajectory,
                          const model::trajectory &pattern_trajectory) {
    // --------------
    // filter
    // --------------

    static const auto min_length = 2;

    if (bg::num_points(input_trajectory) <= min_length) {
        logging::is_logging = true;
        LOG_TEXT("input trajectory needs at least "
                         << min_length
                         << " points, but has "
                         << bg::num_points(input_trajectory)
                         << " points");
        logging::is_logging = false;
        return;
    }
    if (bg::num_points(pattern_trajectory) <= min_length) {
        logging::is_logging = true;
        LOG_TEXT("pattern trajectory needs at least "
                         << min_length
                         << " points, but has "
                         << bg::num_points(pattern_trajectory)
                         << " points");
        logging::is_logging = false;
        return;
    }


    // --------------
    // preprocessing
    // --------------

    const auto input_mbs = min_bounding_sphere(input_trajectory);
    const auto pattern_mbs = min_bounding_sphere(pattern_trajectory);


    // translate
    model::point pattern_translation_vector(pattern_mbs.center);
    bg::multiply_value(pattern_translation_vector, -1);
    LOG(pattern_translation_vector);

    model::trajectory pattern_translated;
    trans::translate_transformer<double, 2, 2> pattern_translate(bg::get<0>(pattern_translation_vector),
                                                                 bg::get<1>(pattern_translation_vector));
    boost::geometry::transform(pattern_trajectory, pattern_translated, pattern_translate);
    LOG(pattern_translated);


    model::point input_translation_vector(input_mbs.center);
    bg::multiply_value(input_translation_vector, -1);
    LOG(input_translation_vector);

    model::trajectory input_translated;
    trans::translate_transformer<double, 2, 2> input_translate(bg::get<0>(input_translation_vector),
                                                               bg::get<1>(input_translation_vector));
    boost::geometry::transform(input_trajectory, input_translated, input_translate);
    LOG(input_translated);


    // scale
    const double normalized_size = 100;

    model::trajectory pattern_scaled;
    trans::scale_transformer<double, 2, 2> pattern_scale(normalized_size / pattern_mbs.radius);
    boost::geometry::transform(pattern_translated, pattern_scaled, pattern_scale);
    LOG(pattern_scaled);

    model::trajectory input_scaled;
    trans::scale_transformer<double, 2, 2> input_scale(normalized_size / input_mbs.radius);
    boost::geometry::transform(input_translated, input_scaled, input_scale);
    LOG(input_scaled);


    // rotate
//    const model::point pattern_orientation = bg::return_centroid<model::point>(pattern_scaled);
//    const model::point input_orientation = bg::return_centroid<model::point>(input_scaled);

    // use first point if centroid is equal to center of bounding sphere
    const model::point pattern_orientation = pattern_scaled[0];
    const model::point input_orientation = input_scaled[0];

    const auto pattern_angle = x_angle(pattern_orientation);
    const auto input_angle = x_angle(input_orientation);
    using rotate_transformer = trans::rotate_transformer<bg::radian, double, 2, 2>;
    rotate_transformer input_rotate(pattern_angle - input_angle);
    model::trajectory input_rotated;
    bg::transform(input_scaled, input_rotated, input_rotate);


    const model::trajectory &transformed_input = input_rotated;
    const model::trajectory &transformed_pattern = pattern_scaled;

    // ---------------------
    // compare trajectories
    // ---------------------

    // configure distance measurement
    trajecmp::distance::neighbours_percentage_range neighbours(0.1);

    // calculate distance
    const auto distance = trajecmp::distance::modified_hausdorff(neighbours)(transformed_input, transformed_pattern);
    logging::is_logging = true;
    LOG(distance);

    const auto max_distance = normalized_size * 0.20;
    LOG(max_distance);

    // decide similarity based on distance
    const auto is_similar = (distance <= max_distance);

    LOG(is_similar);
    LOG_SEP();
    logging::is_logging = false;


    // ---------------------
    // visualize
    // ---------------------

    const auto visualization_size = 300;

    model::trajectory input_visualization_scaled;
    trans::scale_transformer<double, 2, 2> input_visualization_scale(visualization_size / 2 / normalized_size);
    boost::geometry::transform(transformed_input, input_visualization_scaled, input_visualization_scale);

    model::trajectory input_visualization_translated;
    trans::translate_transformer<double, 2, 2> input_visualization_translate(visualization_size / 2,
                                                                             visualization_size / 2);
    boost::geometry::transform(input_visualization_scaled,
                               input_visualization_translated,
                               input_visualization_translate);


    model::trajectory pattern_visualization_scaled;
    trans::scale_transformer<double, 2, 2> pattern_visualization_scale(visualization_size / 2 / normalized_size);
    boost::geometry::transform(transformed_pattern, pattern_visualization_scaled, pattern_visualization_scale);

    model::trajectory pattern_visualization_translated;
    trans::translate_transformer<double, 2, 2> pattern_visualization_translate(visualization_size / 2,
                                                                               visualization_size / 2);
    boost::geometry::transform(pattern_visualization_scaled,
                               pattern_visualization_translated,
                               pattern_visualization_translate);


    const model::trajectory &visualization_normalized_input = input_visualization_translated;
    const model::trajectory &visualization_normalized_pattern = pattern_visualization_translated;
    const auto visualization_normalized_input_mbs = min_bounding_sphere(visualization_normalized_input);
    const auto visualization_normalized_pattern_mbs = min_bounding_sphere(visualization_normalized_pattern);
    // LOG(visualization_normalized_input_mbs);
    // LOG(visualization_normalized_pattern_mbs);

    // TrajectorySvg svg("example.svg", visualization_size, visualization_size);
    // svg.add(visualization_normalized_input, "visualization_normalized_input");
    // svg.add(visualization_normalized_pattern, "visualization_normalized_pattern");

    draw_trajectory(renderer, visualization_normalized_pattern, color_code::yellow);
    draw_trajectory(renderer,
                    visualization_normalized_input,
                    is_similar ? color_code::green : color_code::red);

    const auto centroid_input = bg::return_centroid<model::point>(visualization_normalized_input);
    const auto centroid_pattern = bg::return_centroid<model::point>(visualization_normalized_pattern);
    draw_line(renderer,
              visualization_normalized_pattern_mbs.center,
              centroid_pattern,
              color_code::yellow);
    draw_line(renderer,
              visualization_normalized_input_mbs.center,
              centroid_input,
              is_similar ? color_code::green : color_code::red);

}


auto is_rerender = true;
auto is_recording_trajectory = false;
model::trajectory trajectory;
const model::trajectory pattern_letter_L{{0, 0},
                                  {0, 2},
                                  {1, 2}};

const model::trajectory pattern_square{{0, 0},
                                {0, 1},
                                {1, 1},
                                {1, 0},
                                {0, 0}};

void one_iter() {
    if (is_rerender) {
        is_rerender = false;
        // const model::trajectory trajectory{{320, 200},
        //                             {300, 240},
        //                             {340, 240}};

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        draw_trajectory(renderer, trajectory);
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                done = SDL_TRUE;
                break;
            case SDL_MOUSEBUTTONDOWN:
                LOG_TEXT("SDL_MOUSEBUTTONDOWN");
                is_rerender = true;
                is_recording_trajectory = true;
                bg::append(trajectory, model::point(event.motion.x, event.motion.y));
                break;
            case SDL_MOUSEMOTION:
                if (is_recording_trajectory) {
                    LOG_TEXT("SDL_MOUSEMOTION");
                    is_rerender = true;
                    bg::append(trajectory, model::point(event.motion.x, event.motion.y));
                }
                break;
            case SDL_MOUSEBUTTONUP:
                LOG_TEXT("SDL_MOUSEBUTTONUP");
                is_rerender = false;
                is_recording_trajectory = false;

                // START_LOGGING("compare_trajectories");
//                compare_trajectories(trajectory, pattern_letter_L);
//                compare_trajectories(trajectory, pattern_square);
                compare_trajectories(trajectory, input::pattern);
                // STOP_LOGGING();

                bg::clear(trajectory);
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) == 0) {
            emscripten_set_main_loop(one_iter, 0, 1);
            // emscripten_set_main_loop(one_iter, 0, 1);
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    // SDL_Quit();
    return 0;
}