#include <trajecmp/transform/translate_and_scale.hpp>
#include "record_trajectory_sdl2_framework.hpp"

void record_trajectory_sdl2_framework::renderer_clear() {
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}

void record_trajectory_sdl2_framework::display() {
    if (!_is_rerender) {
        return;
    }
    renderer_clear();

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
                handle_input_trajectory_part(_trajectory);
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

void record_trajectory_sdl2_framework::transform_for_visualization(model::trajectory &trajectory) {
    namespace pm = pattern_matching;
    using trajecmp::transform::scale_and_translate;
    int w, h;
    SDL_GetRendererOutputSize(_renderer, &w, &h);
    const int center_x = w / 2;
    const int center_y = h / 2;
    const model::vector center(center_x, center_y);
    const auto visualization_size = std::min(w, h) - 20;
    scale_and_translate(visualization_size / pm::normalized_size,
                        center,
                        trajectory);
}

model::point record_trajectory_sdl2_framework::get_visualization_center() {
    int w, h;
    SDL_GetRendererOutputSize(_renderer, &w, &h);
    return { w / 2, h / 2};

}

void record_trajectory_sdl2_framework::handle_input_trajectory_part(
        const model::trajectory &input_part) {
}
