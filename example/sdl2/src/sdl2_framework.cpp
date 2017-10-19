#include <iostream>
#include "sdl2_framework.hpp"

sdl2_framework::sdl2_framework() : _is_running(false) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error("Could not initialize SDL2");
    }
    _window = SDL_CreateWindow("Render Line",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               640,
                               480,
                               SDL_WINDOW_SHOWN);
    if (!_window) {
        throw std::runtime_error("Could not create SDL2 window");
    }
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
}

sdl2_framework::~sdl2_framework() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void sdl2_framework::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handle_event(event);
    }
}

void sdl2_framework::handle_event(const SDL_Event &event) {
    if (event.type == SDL_QUIT) {
        _is_running = false;
    }
}

void sdl2_framework::start() {
    _is_running = true;
    while (_is_running) {
        handle_events();
        const auto frame_time = SDL_GetTicks();
        display();
        const auto frame_time_after = SDL_GetTicks();
        const auto min_frame_time = 1000 / frames_per_sec;
        if (frame_time_after - frame_time < min_frame_time) {
            SDL_Delay(min_frame_time - (frame_time_after - frame_time));
        }
    }
}
