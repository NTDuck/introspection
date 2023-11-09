#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>


/**
 * @brief A wrapper class representing a texture.
*/
class BaseTextureWrapper {
    public:
        BaseTextureWrapper();
        ~BaseTextureWrapper();

        virtual void initAbstract(const std::filesystem::path xmlPath);
        void blit();
        virtual void render();

        void setRGB(Uint8 r, Uint8 g, Uint8 b);
        void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        void setAlpha(Uint8 alpha);
        void setRGBA(SDL_Color col);

        virtual void move();
        virtual bool validateMove();

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


class NonStaticTextureWrapper : public BaseTextureWrapper {
    public:
        NonStaticTextureWrapper();
        ~NonStaticTextureWrapper();

        void initAbstract(const std::filesystem::path xmlPath) override;

        void move() override;
        bool validateMove() override;

        void onMoveStart();
        void onMoveEnd() override;

    private:
        void updateAnimation();
        void resetAnimation(const std::string nextAnimationState);

        /**
         * @brief Maps the animation state with its respective starting and stopping GIDs.
         * @todo Consider implementing `enum` instead of `std::string`.
         * @todo Optimize retrieval.
        */
        std::unordered_map<std::string, std::pair<int, int>> rotatingGIDs;
        std::string currAnimationState;
        int currAnimationGID;

        /**
         * @brief The maximum number of tiles per dimension in the tileset.
        */
        SDL_Point srcRectCount;

        /**
         * @brief Indicates the number of frames a sprite should last before switching to the next. Should be treated as a constant.
        */
        int animationUpdateRate;
};


class StaticTextureWrapper : public BaseTextureWrapper {
    // bruf
};