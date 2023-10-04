#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include <utils/sdl2.h>


namespace utils {
    SDL_Surface* optimize(const std::string path, const SDL_PixelFormat* fmt) {
        SDL_Surface* optimized = nullptr;
        SDL_Surface* raw = IMG_Load(path.c_str());
        if (raw == nullptr) {
            std::cout << "Failed to load image: " << IMG_GetError();
            return nullptr;
        }

        optimized = SDL_ConvertSurface(raw, fmt, 0);
        if (optimized == nullptr) {
            std::cout << "Failed to optimize image: " << SDL_GetError();
            return nullptr;
        }

        SDL_FreeSurface(raw);
        return optimized;
    }

    SDL_Texture* loadTexture(const std::string path, SDL_Renderer* renderer) {
        SDL_Texture* newTexture = nullptr;
        SDL_Surface* rawSurface = IMG_Load(path.c_str());
        if (rawSurface == nullptr) {
            std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        newTexture = SDL_CreateTextureFromSurface(renderer, rawSurface);
        if (newTexture == nullptr) {
            std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        SDL_FreeSurface(rawSurface);
        return newTexture;
    }
}