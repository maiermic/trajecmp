#ifndef DRAW_HPP
#define DRAW_HPP

#include "color.hpp"
#include "model.hpp"

#include <SDL2/SDL.h>

void draw_trajectory(SDL_Renderer *renderer,
                     const model::trajectory &trajectory,
                     const rgb color = rgb {255, 255, 255});

void draw_line(SDL_Renderer *renderer,
               const model::point &start,
               const model::point &end,
               const rgb color = rgb {255, 255, 255});

void draw_box(SDL_Renderer *renderer, const model::point &center, int size,
              const rgb &color);


#endif //DRAW_HPP
