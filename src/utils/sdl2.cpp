#include <SDL.h>
#include <SDL_image.h>

#include <utils/sdl2.h>


namespace utils {
    SDL_Surface* optimize(const std::string path, const SDL_PixelFormat* fmt) {
        SDL_Surface* optimized = NULL;
        SDL_Surface* raw = IMG_Load(path.c_str());
        if (raw == NULL) {
            std::cout << "Failed to load image: " << IMG_GetError();
            return NULL;
        }

        optimized = SDL_ConvertSurface(raw, fmt, 0);
        if (optimized == NULL) {
            std::cout << "Failed to optimize image: " << SDL_GetError();
            return NULL;
        }

        SDL_FreeSurface(raw);
        return optimized;
    }
}