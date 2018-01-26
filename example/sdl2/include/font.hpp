#ifndef TRAJECMP_FONT_HPP
#define TRAJECMP_FONT_HPP

#include <SDL_ttf.h>

inline TTF_Font *open_default_font() {
    TTF_Init();
    return TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 18);
}

#endif //TRAJECMP_FONT_HPP
