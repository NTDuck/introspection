#pragma once

#include <vector>

#include <SDL.h>
#include <auxiliaries/globals.hpp>


/**
 * @brief A wrapper class representing a texture.
*/
class TextureWrapper {
    public:
        TextureWrapper(SDL_Point destCoords);
        ~TextureWrapper();

        void init(const std::string path);
        void blit();
        void render();

        void setRGB(Uint8 r, Uint8 g, Uint8 b);
        void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        void setAlpha(Uint8 alpha);
        void setRGBA(SDL_Color col);

        void move();

    protected:
        SDL_Rect getDestRectFromCoords (const SDL_Point coords);
        void moveCleanup();

        SDL_Texture* texture = nullptr;   // expects a single tileset
        SDL_Point destCoords;
        SDL_Rect destRect;
        SDL_Point* nextDestCoords = nullptr;
        SDL_Rect* nextDestRect = nullptr;
        SDL_Rect srcRect;
        SDL_Point velocity;   // between -1 and 1
        const SDL_Point VELOCITY = {1, 1};

        double angle = 0;
        SDL_Point* center = nullptr;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
};
