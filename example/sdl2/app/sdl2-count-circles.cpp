#include <iostream>
#include <sstream>
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
#include <trajecmp/transform/translate_and_scale.hpp>
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
#include "notification_box.hpp"
#include "font.hpp"


class framework : public record_trajectory_sdl2_framework {
    notification_box _notification_box;

public:
    framework() : _notification_box(open_default_font()) {
        _notification_box.message("draw circles");
    }

    void handle_input_trajectory(model::trajectory input) {
        namespace bg = boost::geometry;
        using trajecmp::transform::douglas_peucker;
        using trajecmp::transform::translate_and_scale_using_mbs;
        using trajecmp::gesture::estimate_circle_segment;
        using trajecmp::gesture::get_average_radius_factor_sized_circle_trajectory;
        using pattern_matching::normalized_size;
        using pattern_matching::modified_hausdorff_info;

        input = douglas_peucker(3)(input);
        if (bg::num_points(input) < 3) return;
        auto mbs = trajecmp::geometry::min_bounding_sphere(input);
        translate_and_scale_using_mbs(normalized_size, mbs, input);
        const auto c = estimate_circle_segment(input, mbs);
        auto pattern = get_average_radius_factor_sized_circle_trajectory(
                normalized_size, input, mbs, c);
        draw_comparison_data(modified_hausdorff_info(input, pattern), input,
                             pattern, c);
    }

    void draw_comparison_data(
            const boost::geometry::distance_info_result<model::point> &distance,
            model::trajectory &input_trajectory,
            model::trajectory &pattern_trajectory,
            const trajecmp::gesture::circle_segment_info<float, model::point> &c) {
        using pattern_matching::normalized_size;
        const auto is_similar = distance.real_distance < normalized_size * 0.20;
        model::trajectory distance_trajectory{
                distance.projected_point1,
                distance.projected_point2,
        };
        transform_for_visualization(pattern_trajectory);
        transform_for_visualization(input_trajectory);
        transform_for_visualization(distance_trajectory);
        renderer_clear();
        draw_trajectory(_renderer, pattern_trajectory, color_code::yellow);
        draw_trajectory(_renderer, input_trajectory,
                        is_similar ? color_code::green
                                   : color_code::red);

        draw_trajectory(_renderer, distance_trajectory, color_code::pink);
        draw_box(_renderer, get_visualization_center(), 10, color_code::gray);

        std::ostringstream variables_str;
        variables_str << "distance: " << std::fixed
                      << std::setprecision(2) << distance.real_distance;
        if (is_similar) {
            variables_str << ", circle count: " << c.circle_count()
                          << ", full circle count: " << c.full_circle_count();
            _notification_box.message("matched circles, " + variables_str.str());
            _notification_box.error("");
        } else {
            _notification_box.message("draw circles");
            _notification_box.error("mismatched circles, " + variables_str.str());
        }
        _notification_box.render(_renderer);
        SDL_RenderPresent(_renderer);
        is_rerender(false);
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
    framework().start();
    return 0;
}