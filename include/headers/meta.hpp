#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>


/**
 * @brief A wrapper class representing a texture.
*/
class TextureWrapper {
    public:
        TextureWrapper();
        ~TextureWrapper();

        void init(const std::filesystem::path xmlPath);
        void blit();
        virtual void render();

        void setRGB(Uint8 r, Uint8 g, Uint8 b);
        void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        void setAlpha(Uint8 alpha);
        void setRGBA(SDL_Color col);

        virtual void move();

    protected:
        SDL_Rect getDestRectFromCoords (const SDL_Point coords);
        virtual void onMoveEnd();

        SDL_Texture* texture = nullptr;   // expects a single tileset
        SDL_Point destCoords;
        SDL_Rect destRect;
        SDL_Point* nextDestCoords = nullptr;
        SDL_Rect* nextDestRect = nullptr;
        SDL_Rect srcRect;
        SDL_Point velocity;   // between -1 and 1
        SDL_Point VELOCITY;

        double angle = 0;
        SDL_Point* center = nullptr;
        SDL_RendererFlip flip = SDL_FLIP_NONE;

        std::unordered_map<std::string, std::string> properties;
};
