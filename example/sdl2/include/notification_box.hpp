#ifndef TRAJECMP_NOTIFICATION_BOX_HPP
#define TRAJECMP_NOTIFICATION_BOX_HPP

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <text_renderer.hpp>

class notification_box {
    text_renderer _message_renderer;
    text_renderer _error_message_renderer;
public:
    explicit notification_box(TTF_Font *font);

    void render(SDL_Renderer *renderer);
    
    void message(const std::string &text);
    
    void error(const std::string &text);
};


#endif //TRAJECMP_NOTIFICATION_BOX_HPP
