#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <pattern_matching.hpp>
#include <sdl2_framework.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/extensions/strategies/cartesian/distance_info.hpp>
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "../../logging.hpp"

class framework : public sdl2_framework {
    bool _is_rerender = true;
    bool _is_recording_trajectory = false;
    model::trajectory _trajectory;
public:
    framework() {
        subscribe_to_pattern_matching();
    }

    void subscribe_to_pattern_matching() {
        using trajecmp::util::subscribe_with_latest_from;
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;

        pm::distance_stream
        .with_latest_from(
                [&](bg::distance_info_result<model::point> distance,
                    model::trajectory input_trajectory,
                    model::trajectory pattern_trajectory) {
                    static const auto visualization_size = 300;
                    const auto transform_for_visualization = trajecmp::functional::pipe(
                            trajecmp::transform::scale_to_const<visualization_size>(pm::normalized_size),
                            trajecmp::transform::translate_by(model::vector(visualization_size / 2, visualization_size / 2))
                    );
                    const auto is_similar = distance.real_distance < pattern_matching::normalized_size * 0.20;
                    draw_trajectory(_renderer, transform_for_visualization(pattern_trajectory), color_code::yellow);
                    draw_trajectory(_renderer,
                                    transform_for_visualization(input_trajectory),
                                    is_similar ? color_code::green : color_code::red);
                    model::trajectory distance_trajectory {
                            distance.projected_point1,
                            distance.projected_point2,
                    };
                    draw_trajectory(_renderer, transform_for_visualization(distance_trajectory), color_code::pink);
                    std::cout << "distance: " << distance.real_distance << '\n';
                    _is_rerender = false;
                    return 0; // dummy value
                },
                pm::preprocessed_input_trajectory_stream,
                pm::preprocessed_pattern_trajectory_stream
        ).subscribe([](auto _) {});

        pm::pattern_trajectory_subject
                .get_subscriber()
                .on_next(
                        trajecmp::trajectory::circle<model::trajectory>(1)
                                .sample(0, -360, 10)
                );
    }

    void display() override {
        if (!_is_rerender) {
            return;
        }
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);

        draw_trajectory(_renderer, _trajectory);

        SDL_RenderPresent(_renderer);
    }

    void handle_event(const SDL_Event &event) override {
        namespace bg = boost::geometry;
        sdl2_framework::handle_event(event);
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                _is_rerender = true;
                _is_recording_trajectory = true;
                bg::append(_trajectory, model::point(event.motion.x, event.motion.y));
                break;
            case SDL_MOUSEMOTION:
                if (_is_recording_trajectory) {
                    _is_rerender = true;
                    bg::append(_trajectory, model::point(event.motion.x, event.motion.y));
                }
                break;
            case SDL_MOUSEBUTTONUP:
                _is_rerender = false;
                _is_recording_trajectory = false;

                std::cout << "emit trajectory" << _trajectory << '\n';
                pattern_matching::input_trajectory_subject
                        .get_subscriber()
                        .on_next(_trajectory);

                bg::clear(_trajectory);
                break;
        }
    }
};

int main() {
    framework().start();
    return 0;
}