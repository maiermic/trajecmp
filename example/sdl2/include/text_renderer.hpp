#ifndef TRAJECMP_TEXT_RENDERER_HPP
#define TRAJECMP_TEXT_RENDERER_HPP

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class text_renderer {
    std::string _text;
    TTF_Font *_font;
    SDL_Texture *_texture;
    SDL_Rect _texture_rect;
    SDL_Color _text_color;
    SDL_Point _text_pos;

    void create_texture(SDL_Renderer *renderer);

public:
    explicit text_renderer(TTF_Font *_font,
                           const SDL_Point &_text_pos = {0, 0},
                           const SDL_Color &_text_color = {255, 255, 255, 0});

    const std::string &text() const;

    void text(const std::string &_text);

    void render(SDL_Renderer *renderer);

    virtual ~text_renderer();
};


#endif //TRAJECMP_TEXT_RENDERER_HPP
