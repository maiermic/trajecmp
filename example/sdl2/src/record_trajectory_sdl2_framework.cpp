#include "record_trajectory_sdl2_framework.hpp"

void record_trajectory_sdl2_framework::display() {
    if (!_is_rerender) {
        return;
    }
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    draw_trajectory(_renderer, _trajectory);

    SDL_RenderPresent(_renderer);
}

void record_trajectory_sdl2_framework::handle_event(const SDL_Event &event) {
    namespace bg = boost::geometry;
    sdl2_framework::handle_event(event);
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            _is_rerender = true;
            _is_recording_trajectory = true;
            bg::append(_trajectory,
                       model::point(event.motion.x, event.motion.y));
            break;
        case SDL_MOUSEMOTION:
            if (_is_recording_trajectory) {
                _is_rerender = true;
                bg::append(_trajectory,
                           model::point(event.motion.x, event.motion.y));
            }
            break;
        case SDL_MOUSEBUTTONUP:
            _is_rerender = false;
            _is_recording_trajectory = false;
            handle_input_trajectory(_trajectory);
            bg::clear(_trajectory);
            break;
    }
}

bool record_trajectory_sdl2_framework::is_rerender() const {
    return _is_rerender;
}

void record_trajectory_sdl2_framework::is_rerender(bool is_rerender) {
    record_trajectory_sdl2_framework::_is_rerender = is_rerender;
}
