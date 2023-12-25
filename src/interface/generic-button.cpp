#include <interface.hpp>

#include <SDL.h>


GenericButton::GenericButton(std::string const& content, SDL_FPoint const& center) : GenericTextArea<GenericButton>(content, center) {}