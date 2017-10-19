#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <color.hpp>
#include <draw.hpp>
#include <model.hpp>

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "Could not initialise" << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Render Line",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              SDL_WINDOW_SHOWN);
    if(!window)
    {
        std::cout << "Could not create the window" << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);

    circleColor(renderer, 300, 300, 50, 0xFF00FFFF);
    draw_line(renderer, {100, 100}, {200, 200}, color_code::green);

    SDL_RenderPresent(renderer);

    bool run = true;
    while(run)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                run = false;
            }
        }

    }

    SDL_Quit();
    return 0;
}