#ifndef DRAW_HPP
#define DRAW_HPP

#include "color.hpp"
#include "model.hpp"

#include <SDL_stdinc.h>

void draw_trajectory(SDL_Renderer *renderer,
                     const model::trajectory &trajectory,
                     const rgb color = rgb {255, 255, 255}) {
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, SDL_ALPHA_OPAQUE);
    for (auto i = trajectory.begin(); i != trajectory.end();) {
        const model::point &previous = *i;
        ++i;
        if (i == trajectory.end()) break;
        const model::point &current = *i;
        SDL_RenderDrawLine(renderer,
                           (int) boost::geometry::get<0>(current),
                           (int) boost::geometry::get<1>(current),
                           (int) boost::geometry::get<0>(previous),
                           (int) boost::geometry::get<1>(previous));
    }
}

void draw_line(SDL_Renderer *renderer,
               const model::point &start,
               const model::point &end,
               const rgb color = rgb {255, 255, 255}) {
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,
                       (int) boost::geometry::get<0>(start),
                       (int) boost::geometry::get<1>(start),
                       (int) boost::geometry::get<0>(end),
                       (int) boost::geometry::get<1>(end));
}

void draw_box(SDL_Renderer *renderer, const model::point &center, int size,
              const Uint32 color) {
    const auto x = boost::geometry::get<0>(center);
    const auto y = boost::geometry::get<1>(center);
    const auto offset = size / 2;
    boxColor(renderer, x - offset, y - offset, x + offset, y + offset, color);
}


#endif //DRAW_HPP
