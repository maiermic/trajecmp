#include <text_renderer.hpp>

const std::string &text_renderer::text() const {
    return _text;
}

void text_renderer::text(const std::string &text) {
    _text = text;
    if (_texture != nullptr) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
}

text_renderer::~text_renderer() {
    if (_texture != nullptr) {
        SDL_DestroyTexture(_texture);
    }
}

void text_renderer::create_texture(SDL_Renderer *renderer) {
    SDL_Surface *surface =
            TTF_RenderText_Blended(_font, _text.c_str(), _text_color);
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
    _texture_rect = {_text_pos.x, _text_pos.y, surface->w, surface->h};
    SDL_FreeSurface(surface);
}

void text_renderer::render(SDL_Renderer *renderer) {
    if (_text.empty()) {
        return;
    }
    if (_texture == nullptr) {
        create_texture(renderer);
    }
    SDL_RenderCopy(renderer, _texture, nullptr, &_texture_rect);
}

text_renderer::text_renderer(
        TTF_Font *_font,
        const SDL_Point &_text_pos,
        const SDL_Color &_text_color)
        : _font(_font),
          _text_color(_text_color),
          _text_pos(_text_pos) {

}
