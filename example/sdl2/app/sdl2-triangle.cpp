#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <pattern.hpp>
#include <pattern_matching.hpp>
#include <sdl2_framework.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/extensions/strategies/cartesian/distance_info.hpp>
#include "trajecmp/transform/rearrange_closed_trajectory.hpp"
#include <trajecmp/transform/close.hpp>
#include <notification_box.hpp>
#include "trajecmp/util/angle.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/vector_ostream.hpp"
#include "trajecmp/transform/douglas_peucker.hpp"
#include "trajecmp/transform/translate_and_scale.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"
#include "font.hpp"

model::trajectory get_distance_trajectory(
        const boost::geometry::distance_info_result<model::point> &distance) {
    return {
            distance.projected_point1,
            distance.projected_point2,
    };
}

struct framework : public record_trajectory_sdl2_framework {
    const model::trajectory _pattern;
    notification_box _notification_box;

    explicit framework(const model::trajectory &pattern)
            : _pattern(pattern), _notification_box(open_default_font()) {
        _notification_box.message("draw triangle");
    }

    void handle_input_trajectory(model::trajectory input) override {
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::transform::rearrange_closed_input_using_pattern_corners;
        using pattern_matching::modified_hausdorff_info;
        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 3) return;
        trajecmp::transform::translate_using_mbs_and_scale_using_mbb(
                trajecmp::geometry::min_bounding_box(_pattern),
                input);
        bg::append(input, *std::begin(input));
        input = rearrange_closed_input_using_pattern_corners(_pattern, input);
        const auto distance = modified_hausdorff_info(_pattern, input);
        auto pattern_trajectory = _pattern;
        renderer_clear();
        draw_trajectories(pattern_trajectory, input, distance);
        _notification_box.render(_renderer);
        SDL_RenderPresent(_renderer);
        is_rerender(false);
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
        const auto is_similar = distance.real_distance <
                                pattern_matching::normalized_size *
                                0.20;
        if (is_similar) {
            _notification_box.message("matched triangle, draw triangle again");
            _notification_box.error("");
        } else {
            _notification_box.message("draw triangle again");
            _notification_box.error("mismatched triangle");
        }
        draw_trajectory(_renderer, pattern_trajectory, color_code::yellow);
        draw_trajectory(_renderer, input_trajectory,
                        is_similar ? color_code::green : color_code::red);
        draw_trajectory(_renderer, distance_trajectory, color_code::pink);
        draw_box(_renderer, pattern_trajectory.at(0), 10, color_code::orange);
        draw_box(_renderer, input_trajectory.at(0), 10, color_code::orange);
        draw_box(_renderer, pattern_trajectory.at(1), 10, color_code::blue);
        draw_box(_renderer, input_trajectory.at(1), 10, color_code::blue);
    }

    void display() override {
        record_trajectory_sdl2_framework::display();
        if (is_rerender()) {
            _notification_box.render(_renderer);
            SDL_RenderPresent(_renderer);
        }
        is_rerender(false);
    }

};

int main() {
    logging::is_logging = true;
    model::trajectory pattern = pattern::triangle_up;
    trajecmp::transform::translate_and_scale_using_mbs(
            pattern_matching::normalized_size, pattern);
    framework(pattern).start();
    return 0;
}