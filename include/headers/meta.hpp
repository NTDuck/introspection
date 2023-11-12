#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include <auxiliaries/globals.hpp>


/**
 * @brief An abstract class representing a texture. Supports basic operations.
*/
class BaseTextureWrapper {
    public:
        BaseTextureWrapper();
        ~BaseTextureWrapper();

        virtual void init_(const std::filesystem::path xmlPath);
        virtual void render();

        virtual void onWindowChange();
        virtual void onLevelChange(const globals::levelData::Texture& texture);

        void setRGB(Uint8 r, Uint8 g, Uint8 b);
        void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        void setAlpha(Uint8 alpha);
        void setRGBA(SDL_Color col);

        bool operator<(const BaseTextureWrapper& other) const;
        bool operator==(const BaseTextureWrapper& other) const;

        SDL_Point destCoords_getter() const;
        SDL_Rect getDestRectFromCoords(const SDL_Point coords);

    protected:
        SDL_Texture* texture = nullptr;   // expects a single tileset
        SDL_Point destCoords;
        SDL_Rect destRect;
        SDL_Rect srcRect;

        double angle = 0;
        SDL_Point* center = nullptr;
        SDL_RendererFlip flip = SDL_FLIP_NONE;

        std::unordered_map<std::string, std::string> properties;
};


/**
 * @brief An abstract class representing a texture that updates its animation.
*/
class AnimatedTextureWrapper : public BaseTextureWrapper {
    public:
        AnimatedTextureWrapper();
        ~AnimatedTextureWrapper();

        void init_(const std::filesystem::path xmlPath) override;

        void updateAnimation();
        void resetAnimation(const std::string nextAnimationState);

        /**
         * @note Allow public access to encapsulated internal state.
        */
        bool isNextTileReached = false;

    protected:
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


/**
 * @brief An abstract class representing a texture that updates its animation and is able to change its position.
*/
class AnimatedDynamicTextureWrapper : public AnimatedTextureWrapper {
    public:
        AnimatedDynamicTextureWrapper();
        ~AnimatedDynamicTextureWrapper();

        void init_(const std::filesystem::path xmlPath) override;
        void onLevelChange(const globals::levelData::Texture& texture) override;

        virtual void move();
        virtual bool validateMove();

        void onMoveStart();
        void onMoveEnd();

        SDL_Point* nextDestCoords = nullptr;
        SDL_Rect* nextDestRect = nullptr;

    protected:
        SDL_Point velocity;   // between -1 and 1
        SDL_Point VELOCITY;
};


/**
 * @brief An abstract class representing a texture that updates its animation and is NOT able to change its position.
*/
class AnimatedStaticTextureWrapper : public AnimatedTextureWrapper {
    // poof
};