#pragma once

#include <iostream>
#include <string>

#include <SDL.h>


namespace utils {
    SDL_Surface* optimize(const std::string path, const SDL_PixelFormat* fmt);
}