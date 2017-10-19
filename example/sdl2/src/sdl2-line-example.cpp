#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>
#include <sdl2_framework.hpp>

class framework : public sdl2_framework {
public:
    void display() override {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);

        circleColor(_renderer, 300, 300, 50, 0xFF00FFFF);
        draw_line(_renderer, {100, 100}, {200, 200}, color_code::green);

        SDL_RenderPresent(_renderer);
    }
};

int main() {
    framework().start();
    return 0;
}