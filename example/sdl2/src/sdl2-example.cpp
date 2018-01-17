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
#include <record_trajectory_sdl2_framework.hpp>
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "../../logging.hpp"

void translate_and_scale(model::trajectory &trajectory) {
    namespace pm = pattern_matching;
    using trajecmp::transform::translate_by;
    using trajecmp::geometry::negative_vector_of;
    using trajecmp::transform::scale_to_const;

    const auto mbs = trajecmp::geometry::min_bounding_sphere(trajectory);
    trajectory = translate_by(negative_vector_of(mbs.center))(trajectory);
    trajectory = scale_to_const<pm::normalized_size>(mbs.radius * 2)(trajectory);
}

class framework : public record_trajectory_sdl2_framework {
    const model::trajectory _pattern;

public:
    framework(const model::trajectory &pattern) : _pattern(pattern) {}

    bool preprocess_input_trajectory(model::trajectory &input) {
        using boost::geometry::num_points;

        input = trajecmp::transform::douglas_peucker(3)(input);
        if (num_points(input) < num_points(_pattern)) return false;
        translate_and_scale(input);
        return true;
    }

    void handle_input_trajectory(model::trajectory input) {
        if (preprocess_input_trajectory(input)) {
            draw_comparison_data(
                    pattern_matching::modified_hausdorff_info(input, _pattern),
                    input,
                    _pattern
            );
        }
    }

    void draw_comparison_data(
            boost::geometry::distance_info_result <model::point> distance,
            model::trajectory input_trajectory,
            model::trajectory pattern_trajectory) {
        static const auto visualization_size = 300;
        const auto transform_for_visualization = trajecmp::functional::pipe(
                trajecmp::transform::scale_to_const<visualization_size>(
                        pattern_matching::normalized_size),
                trajecmp::transform::translate_by(
                        model::vector(visualization_size / 2,
                                      visualization_size / 2))
        );
        const auto is_similar = distance.real_distance <
                                pattern_matching::normalized_size * 0.20;
        draw_trajectory(_renderer,
                        transform_for_visualization(pattern_trajectory),
                        color_code::yellow);
        draw_trajectory(_renderer,
                        transform_for_visualization(input_trajectory),
                        is_similar ? color_code::green : color_code::red);
        model::trajectory distance_trajectory{
                distance.projected_point1,
                distance.projected_point2,
        };
        draw_trajectory(_renderer,
                        transform_for_visualization(distance_trajectory),
                        color_code::pink);
        std::cout << "distance: " << distance.real_distance << '\n';
        SDL_RenderPresent(_renderer);
        is_rerender(false);
    }

};

int main() {
    model::trajectory pattern = ::pattern::letter_L;
    translate_and_scale(pattern);
    framework(pattern).start();
    return 0;
}