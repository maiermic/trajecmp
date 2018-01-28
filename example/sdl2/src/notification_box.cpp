#include "notification_box.hpp"

#include <iostream>

notification_box::notification_box(TTF_Font *font)
        : _message_renderer(font, {10, 10}),
          _error_message_renderer(font, {10, 30}, {255, 0, 0, 0}) {}


void notification_box::render(SDL_Renderer *renderer) {
    _message_renderer.render(renderer);
    _error_message_renderer.render(renderer);
}

void notification_box::message(const std::string &text) {
    _message_renderer.text(text);
}

void notification_box::error(const std::string &text) {
    _error_message_renderer.text(text);
}
