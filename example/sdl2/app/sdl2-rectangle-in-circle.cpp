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
#include "trajecmp/geometry/point/arithmetic.hpp"
#include "trajecmp/gesture/circle.hpp"
#include "trajecmp/gesture/rectangle.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/transform/douglas_peucker.hpp"
#include "trajecmp/transform/translate_and_scale.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"
#include "notification_box.hpp"

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
    enum class state {
        match_circle,
        match_rectangle
    };
    state _state = state::match_circle;
    model::trajectory _beautified_circle_trajectory;
    notification_box _notification_box;
    trajecmp::geometry::hyper_sphere_of<model::trajectory> _mbs_of_circle_input;
public:
    framework() : _notification_box(nullptr) {
        TTF_Init();
        const char *font_path = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";
        TTF_Font *font = TTF_OpenFont(font_path, 18);
        _notification_box = notification_box(font);
        _notification_box.message("draw circle");
    }

    ~framework() override {
        TTF_Quit();
    }

    void handle_input_trajectory(model::trajectory input) override {
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::geometry::min_bounding_sphere;
        using trajecmp::transform::translate_and_scale_using_mbs;
        using trajecmp::gesture::get_rectangle_comparison_data;
        using trajecmp::geometry::point::operator-;
        using pattern_matching::normalized_size;

        // shared preprocessing of input trajectory
        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 3) return;
        auto mbs = min_bounding_sphere(input);
        if (_state == state::match_rectangle) {
            const auto d = bg::distance(mbs.center, _mbs_of_circle_input.center);
            if (_mbs_of_circle_input.radius < d + mbs.radius) {
                _notification_box.message("draw circle again");
                _notification_box.error("input outside of circle");
                _state = state::match_circle;
                is_rerender(true);
                return;
            }
        }
        const auto original_mbs = mbs;
        translate_and_scale_using_mbs(normalized_size, mbs, input);
        if (_state == state::match_circle) {
            auto circle_data = get_circle_comparison_data(input, mbs);
            const bool is_circle = circle_data.distance.real_distance <
                                      normalized_size * 0.20;
            if (is_circle) {
                _beautified_circle_trajectory =
                        circle_data.preprocessed_pattern_trajectory;
                trajecmp::transform::scale_and_translate(
                        original_mbs.radius * 2 / normalized_size,
                        original_mbs.center,
                        _beautified_circle_trajectory);
                renderer_clear();
                draw_beautified_circle_trajectory();
                SDL_RenderPresent(_renderer);
                _state = state::match_rectangle;
                _mbs_of_circle_input = original_mbs;
                _notification_box.message("draw rectangle in circle");
                _notification_box.error("");
            } else {
                _notification_box.message("draw circle");
                _notification_box.error("mismatched circle");
            }
        } else {
            bg::append(input, *std::begin(input));
            auto rectangle_data = get_rectangle_comparison_data(
                    input, pattern_matching::modified_hausdorff_info);
            const bool is_rectangle = rectangle_data.distance.real_distance <
                                      normalized_size * 0.20;
            if (is_rectangle) {
                renderer_clear();
                draw_beautified_circle_trajectory();
                auto beautified_rectangle_trajectory =
                        rectangle_data.preprocessed_pattern_trajectory;
                trajecmp::transform::scale_and_translate(
                        original_mbs.radius * 2 / normalized_size,
                        original_mbs.center,
                        beautified_rectangle_trajectory);
                draw_trajectory(_renderer, beautified_rectangle_trajectory,
                                color_code::orange);
                const auto mbs_b_circle = min_bounding_sphere(
                        _beautified_circle_trajectory);
                const auto mbs_b_rectangle = min_bounding_sphere(
                        beautified_rectangle_trajectory);
                const auto d = bg::distance(mbs_b_rectangle.center,
                                            mbs_b_circle.center);
                if (mbs_b_circle.radius < d + mbs_b_rectangle.radius) {
                    _notification_box.message("draw circle again");
                    _notification_box.error("beautified rectangle outside of circle");
                } else {
                    _notification_box.message("matched rectangle in circle, draw circle again");
                    _notification_box.error("");
                }
                _notification_box.render(_renderer);
                SDL_RenderPresent(_renderer);
                is_rerender(false);
                _state = state::match_circle;
                return;
            } else {
                _notification_box.message("draw circle again");
                _notification_box.error("mismatched rectangle");
                _state = state::match_circle;
            }
        }
        is_rerender(true);
    }

    void draw_beautified_circle_trajectory()  {
        draw_trajectory(_renderer, _beautified_circle_trajectory,
                        color_code::blueViolet);
    }

    void display() override {
        record_trajectory_sdl2_framework::display();
        if (is_rerender()) {
            _notification_box.render(_renderer);
            if (_state == state::match_rectangle) {
                draw_beautified_circle_trajectory();
            }
            SDL_RenderPresent(_renderer);
        }
        is_rerender(false);
    }
};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}