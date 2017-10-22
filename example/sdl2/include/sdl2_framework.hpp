#ifndef TRAJECMP_SDL2FRAMEWORK_HPP
#define TRAJECMP_SDL2FRAMEWORK_HPP


#include <SDL2/SDL.h>

class sdl2_framework {
    bool _is_running;
    static const int frames_per_sec = 60;
protected:
    SDL_Window *_window;
    SDL_Renderer *_renderer;
public:
    sdl2_framework();

    virtual ~sdl2_framework();

    virtual void display()= 0;

    virtual void handle_events();

    void start();

    virtual void handle_event(const SDL_Event &event);
};


#endif //TRAJECMP_SDL2FRAMEWORK_HPP
