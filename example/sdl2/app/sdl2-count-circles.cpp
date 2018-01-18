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
    LOG_SEP();
    const auto pattern_trajectory =
            trajecmp::trajectory::circle<model::trajectory>(pm::normalized_size / 2)
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

        pm::input_trajectory_stream
                .map(trajecmp::transform::douglas_peucker(3))
                .filter(trajecmp::predicate::has_min_num_points(3))
                .map(get_circle_comparison_data)
                .subscribe([&](const circle_comparison_data &data) {
                    const model::trajectory &input_trajectory =
                            data.preprocessed_input_trajectory;
                    const model::trajectory &pattern_trajectory =
                            data.preprocessed_pattern_trajectory;
                    const bg::distance_info_result<model::point> &distance =
                            data.distance;
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

                    LOG(pattern_trajectory);
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

                    SDL_RenderPresent(_renderer);
                    _is_rerender = false;
                });
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
    logging::is_logging = true;
    framework().start();
    return 0;
}