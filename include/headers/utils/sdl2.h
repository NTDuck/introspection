#pragma once

#include <iostream>
#include <string>

#include <SDL.h>


namespace utils {
    SDL_Surface* optimize(const std::string path, const SDL_PixelFormat* fmt);
    SDL_Texture* loadTexture(const std::string path, SDL_Renderer* renderer);
}