#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <sdl2_framework.hpp>
#include <trajecmp/geometry/min_bounding_sphere.hpp>
#include "trajecmp/util/angle.hpp"
#include "trajecmp/util/boost_geometry_to_string.hpp"
#include "trajecmp/gesture/circle.hpp"
#include "trajecmp/trajectory/circle.hpp"
#include "trajecmp/transform/translate.hpp"
#include "trajecmp/util/find_local_extrema.hpp"
#include "trajecmp/util/subscribe_with_latest_from.hpp"
#include "trajecmp/util/vector_ostream.hpp"
#include "../../logging.hpp"

class framework : public sdl2_framework {
    using circle_type = trajecmp::trajectory::circle<model::trajectory>;
    float _end_angle;
    float _radius;
    circle_type _circle;
    float _offset;
    const model::point _center;
    model::trajectory _trajectory;
    model::trajectory _full_circle_trajectory;
    trajecmp::geometry::hyper_sphere_of<model::trajectory> _min_bounding_sphere;
    model::trajectory _min_bounding_sphere_trajectory;
    model::point _estimated_center;
public:
    framework() :
            _end_angle(0),
            _radius(150.0f),
            _circle(_radius),
            _offset(_radius + 10.0f),
            _center(_offset, _offset),
            _full_circle_trajectory(
                    trajecmp::transform::translate_by(_center)(
                            circle_type(_radius - 3).sample(0, -360, 10)
                    )
            ) {
    }

    void update() {
        _trajectory =
                trajecmp::transform::translate_by(_center)(
                        _circle.sample(0, _end_angle--, 10)
                );
        _min_bounding_sphere =
                trajecmp::geometry::min_bounding_sphere(_trajectory);
        boost::geometry::append(_min_bounding_sphere_trajectory,
                                _min_bounding_sphere.center);
        _estimated_center =
            trajecmp::gesture::estimate_circle_center(
                    _trajectory.front(),
                    _trajectory.at(_trajectory.size() / 2),
                    _trajectory.back()
            );
    }

    void display() override {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);

        update();
        draw_trajectory(_renderer, _full_circle_trajectory, color_code::white);
        draw_box(_renderer, _center, 5, color_code::white);
        draw_trajectory(_renderer,
                        _min_bounding_sphere_trajectory,
                        color_code::green);
        draw_trajectory(_renderer, _trajectory, color_code::red);
        draw_box(_renderer, _min_bounding_sphere.center, 5, color_code::darkRed);
        draw_box(_renderer, _estimated_center, 10, color_code::orange);
        

        SDL_RenderPresent(_renderer);
        SDL_Delay(100);
    }
};

int main() {
    logging::is_logging = true;
    framework().start();
    return 0;
}