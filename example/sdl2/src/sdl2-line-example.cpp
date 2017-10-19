#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <sdl2_framework.hpp>
#include <boost/geometry/algorithms/append.hpp>

class framework : public sdl2_framework {
    bool _is_rerender = true;
    bool _is_recording_trajectory = false;
    model::trajectory _trajectory;
public:
    void display() override {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);

        circleColor(_renderer, 300, 300, 50, 0xFF00FFFF);
        draw_line(_renderer, {100, 100}, {200, 200}, color_code::green);
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
                bg::clear(_trajectory);
                break;
        }
    }
};

int main() {
    framework().start();
    return 0;
}