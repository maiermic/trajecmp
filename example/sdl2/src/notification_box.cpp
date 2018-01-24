#include "notification_box.hpp"

#include <stdlib.h>
#include <iostream>

notification_box::notification_box(TTF_Font *font)
        : _message_renderer(font, {10, 10}),
          _error_message_renderer(font, {10, 30}, {255, 0, 0, 0}) {}


void notification_box::render(SDL_Renderer *renderer) {
    _message_renderer.render(renderer);
    _error_message_renderer.render(renderer);
//    int offset = 10;
//    get_text_and_rect(renderer, offset + 0, offset + 0, "draw circle", font, &texture1, &rect1);
//    get_text_and_rect(renderer, offset + 0, offset + rect1.y + rect1.h, "mismatch", font, &texture2, &rect2, {255, 0, 0, 0});
//
//    SDL_RenderCopy(renderer, texture1, nullptr, &rect1);
//    SDL_RenderCopy(renderer, texture2, nullptr, &rect2);
}

void notification_box::message(const std::string &text) {
    _message_renderer.text(text);
}

void notification_box::error(const std::string &text) {
    _error_message_renderer.text(text);
}
