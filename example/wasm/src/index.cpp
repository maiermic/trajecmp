#include "emscripten.h"
#include "SDL.h"
#include <vector>
#include <cmath>

#include <boost/geometry.hpp>

#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "../../logging.hpp"

#include "color.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "model.hpp"
#include "pattern_matching.hpp"

#include "trajecmp/util/subscribe_with_latest_from.hpp"


namespace bg = boost::geometry;
namespace trans = boost::geometry::strategy::transform;

using trajecmp::geometry::min_bounding_sphere;


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_bool done = SDL_FALSE;

auto is_rerender = true;
auto is_recording_trajectory = false;
model::trajectory trajectory;

void one_iter() {
    if (is_rerender) {
        is_rerender = false;

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

                pattern_matching::input_trajectory_subject
                        .get_subscriber()
                        .on_next(trajectory);

                bg::clear(trajectory);
                break;
        }
    }
}

void subscribe_to_pattern_matching();


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) == 0) {
            subscribe_to_pattern_matching();
            emscripten_set_main_loop(one_iter, 0, 1);
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    return 0;
}

void subscribe_to_pattern_matching() {
    using trajecmp::util::subscribe_with_latest_from;
    namespace pm = pattern_matching;

    pm::distance_stream
        | subscribe_with_latest_from(
                [](auto distance, auto &&input_trajectory, auto &&pattern_trajectory) {
                    static const auto visualization_size = 300;
                    const auto transform_for_visualization = trajecmp::functional::pipe(
                            trajecmp::transform::scale_to_const<visualization_size>(pm::normalized_size),
                            trajecmp::transform::translate_by(model::vector(visualization_size / 2, visualization_size / 2))
                    );
                    const auto is_similar = distance < pattern_matching::normalized_size * 0.20;
                    draw_trajectory(renderer, transform_for_visualization(pattern_trajectory), color_code::yellow);
                    draw_trajectory(renderer,
                                    transform_for_visualization(input_trajectory),
                                    is_similar ? color_code::green : color_code::red);
                },
                pm::preprocessed_input_trajectory_stream,
                pm::preprocessed_pattern_trajectory_stream
        );
}