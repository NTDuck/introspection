#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include <auxiliaries/globals.hpp>


/**
 * @brief An abstract class representing a texture. Supports basic operations.
 * @todo Each concrete class `C(i)` has exactly one static member `S(i)` (an instance of `TilesetData`) that is unique and accessible to all instances within class `C(i)`. All static member `S(i)` should be populated and dealloc-ed with methods from the base class. This will make sure only one `tilesetData` exists regardless of the number of instances of one concrete class, therefore effectively reduce memory overhead. Required for scalability.
*/
class BaseTextureWrapper {
    public:
        BaseTextureWrapper();
        ~BaseTextureWrapper();

        virtual void init_(const std::filesystem::path xmlPath);
        virtual void render();

        virtual void onWindowChange();
        virtual void onLevelChange(const leveldata::TextureData& texture);

        void setRGB(Uint8 r, Uint8 g, Uint8 b);
        void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        void setAlpha(Uint8 alpha);
        void setRGBA(SDL_Color col);

        SDL_Point destCoords_getter() const;
        SDL_Rect getDestRectFromCoords(const SDL_Point coords);

    protected:
        SDL_Point destCoords;
        SDL_Rect destRect;
        SDL_Rect srcRect;

        /**
         * @brief Modify `destRect`.
         * @param x,y Shift `destRect`'s coordinates by the corresponding value.
         * @param w,h Multiply `destRect`'s width/height by the value corresponded with that dimension. Also accordingly modify `destRect.x` and `destRect.y` so that the value of `center` does not change. Works separately with `destRectModifier.x` and `destRectModifier.y`.
         * @see BaseTextureWrapper.getDestRectFromCoords() (classmethod)
        */
        SDL_Rect destRectModifier = {0, 0, 1, 1};

        double angle = 0;
        SDL_Point* center = nullptr;
        SDL_RendererFlip flip = SDL_FLIP_NONE;

        tiledata::AnimatedEntitiesTilesetData tilesetData;
};


/**
 * @brief An abstract class representing a texture that updates its animation.
*/
class AnimatedTextureWrapper : public BaseTextureWrapper {
    public:
        AnimatedTextureWrapper();
        ~AnimatedTextureWrapper();

        void updateAnimation();
        void resetAnimation(const tiledata::AnimatedEntitiesTilesetData::AnimationType animationType);

        /**
         * @note Allow public access to encapsulated internal state.
        */
        bool isNextTileReached = false;

    protected:
        tiledata::AnimatedEntitiesTilesetData::AnimationType currAnimationType;
        int currAnimationGID;
};


/**
 * @brief An abstract class representing a texture that updates its animation and is able to change its position.
*/
class AnimatedDynamicTextureWrapper : public AnimatedTextureWrapper {
    public:
        AnimatedDynamicTextureWrapper();
        ~AnimatedDynamicTextureWrapper();

        void init_(const std::filesystem::path xmlPath) override;
        void onLevelChange(const leveldata::TextureData& texture) override;

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