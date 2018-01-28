#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include "trajecmp/gesture/rectangle.hpp"

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
#include "trajecmp/transform/douglas_peucker.hpp"
#include "../../logging.hpp"
#include "record_trajectory_sdl2_framework.hpp"
#include <font.hpp>
#include <trajecmp/transform/translate_and_scale.hpp>
#include "notification_box.hpp"


struct framework : public record_trajectory_sdl2_framework {
    using rectangle_comparison_data =
    trajecmp::gesture::rectangle_comparison_data<model::trajectory, boost::geometry::distance_info_result<model::point>>;
    notification_box _notification_box;
    int _display_counter;
    float _percentage_of_drawn_input;
    bool _is_recording_trajectory;

public:
    framework() : _notification_box(open_default_font()),
                  _display_counter(0),
                  _percentage_of_drawn_input(0.0f),
                  _is_recording_trajectory(false) {
        _notification_box.message("draw rectangle from the upper left corner downwards");
    }

    void
    handle_input_trajectory_part(const model::trajectory &input_part) override {
        _is_recording_trajectory = true;
        if (_display_counter % 10 == 9) {
            namespace pm = pattern_matching;
            namespace bg = boost::geometry;
            using trajecmp::transform::douglas_peucker;
            using trajecmp::transform::translate_and_scale_using_mbs;
            using trajecmp::gesture::get_rectangle_comparison_data;
            using trajecmp::geometry::min_bounding_box;
            using trajecmp::transform::sub_trajectory;
            using pattern_matching::modified_hausdorff;
            using bg::length;

            model::trajectory input = input_part;
            input = douglas_peucker(3)(input);
            if (bg::num_points(input) < 2) return;
            auto mbs = trajecmp::geometry::min_bounding_sphere(input);
            translate_and_scale_using_mbs(pm::normalized_size, mbs, input);

            const model::trajectory pattern =
                    trajecmp::trajectory::rectangle<model::trajectory>(
                            min_bounding_box(input));
            const double length_of_pattern = length(pattern);
            const double length_of_input = length(input);
            model::trajectory partial_pattern = { pattern.front() };
            float distance = 100.0; // TODO max value
            _percentage_of_drawn_input = 0.0f;
            for (int i = 1; i < pattern.size(); ++i) {
                partial_pattern.push_back(pattern.at(i));
                model::trajectory p;
                if (i < 3) {
                    p = partial_pattern;
                } else {
                    const double l = length_of_input / length(partial_pattern);
                    p = sub_trajectory(partial_pattern, 0.0, std::min(l, 1.0));
                }
                mbs = trajecmp::geometry::min_bounding_sphere(p);
                translate_and_scale_using_mbs(pm::normalized_size, mbs, p);
                const float d = (float) modified_hausdorff(input, p);
                if (d <= distance) {
                    distance = d;
                    _percentage_of_drawn_input =
                            (i < 3)
                            ? i / (pattern.size() - 1.0f)
                            : length_of_input / length_of_pattern;
                }
            }
            if (distance >= 20) {
                _percentage_of_drawn_input = 0.0f;
            }
        }
    }
    
    void handle_input_trajectory(model::trajectory input) override {
        _is_recording_trajectory = false;
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::transform::translate_and_scale_using_mbs;
        using trajecmp::gesture::get_rectangle_comparison_data;
        using pattern_matching::modified_hausdorff_info;

        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 4) return;
        auto mbs = trajecmp::geometry::min_bounding_sphere(input);
        translate_and_scale_using_mbs(pm::normalized_size, mbs, input);
        bg::append(input, *std::begin(input));
        auto data = get_rectangle_comparison_data(input,
                                                  modified_hausdorff_info);
        draw_rectangle_comparison_data(data);
        _display_counter = 0;
        _percentage_of_drawn_input = 0.0f;
    }

    void draw_rectangle_comparison_data(rectangle_comparison_data &data) {
        namespace pm = pattern_matching;
        namespace bg = boost::geometry;
        model::trajectory &input_trajectory =
                data.preprocessed_input_trajectory;
        model::trajectory &pattern_trajectory =
                data.preprocessed_pattern_trajectory;
        const bg::distance_info_result <model::point> &distance = data.distance;
        const auto is_similar = distance.real_distance <
                                pattern_matching::normalized_size *
                                0.20;
        model::trajectory distance_trajectory{
                distance.projected_point1,
                distance.projected_point2,
        };
        transform_for_visualization(input_trajectory);
        transform_for_visualization(pattern_trajectory);
        transform_for_visualization(distance_trajectory);
        renderer_clear();
        draw_trajectory(_renderer, pattern_trajectory, color_code::yellow);
        draw_trajectory(_renderer,
                        input_trajectory,
                        is_similar ? color_code::green
                                   : color_code::red);
        draw_trajectory(_renderer, distance_trajectory, color_code::pink);
        draw_box(_renderer, get_visualization_center(), 10, color_code::gray);
        LOG(distance.real_distance);

        LOG(data.preprocessed_input_corner_indices.min_corner);
        LOG(data.preprocessed_input_corner_indices.max_corner);
        draw_box(_renderer,
                 input_trajectory.at(
                         data.preprocessed_input_corner_indices.min_corner),
                 10,
                 color_code::pink);
        draw_box(_renderer,
                 input_trajectory.at(
                         data.preprocessed_input_corner_indices.max_corner),
                 10,
                 color_code::cyan);
        std::ostringstream variables_str;
        variables_str << "distance: " << std::fixed
                      << std::setprecision(2) << distance.real_distance;
        if (is_similar) {
            _notification_box.message("matched rectangle, " + variables_str.str());
            _notification_box.error("");
        } else {
            _notification_box.message("draw rectangle");
            _notification_box.error("mismatched rectangle, " + variables_str.str());
        }
        _notification_box.render(_renderer);
        SDL_RenderPresent(_renderer);
        is_rerender(false);
    }

    void display() override {
        record_trajectory_sdl2_framework::display();
        if (is_rerender()) {
            if (_is_recording_trajectory) {
                std::ostringstream message;
                message << "percentage of drawn input: " << std::fixed
                        << std::setprecision(2) << _percentage_of_drawn_input;
                _notification_box.message(message.str());
            }
            _notification_box.render(_renderer);
            SDL_RenderPresent(_renderer);
        }
        is_rerender(false);
        ++_display_counter;
    }
};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}