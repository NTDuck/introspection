#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

// #include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief An abstract class representing a texture. Supports basic operations.
 * @todo Each concrete class `C(i)` has exactly one static member `S(i)` (an instance of `TilesetData`) that is unique and accessible to all instances within class `C(i)`. All static member `S(i)` should be populated and dealloc-ed with methods from the base class. This will make sure only one `tilesetData` exists regardless of the number of instances of one concrete class, therefore effectively reduce memory overhead. Required for scalability.
 * @see https://stackoverflow.com/questions/26950274/implementing-crtp-and-issue-with-undefined-reference
*/
template <class T>
class BaseTextureWrapper {
    public:
        static T* instantiate(SDL_Point destCoords);
        virtual ~BaseTextureWrapper();

        static void initialize(const std::filesystem::path xmlPath);
        static void terminate();

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(const leveldata::TextureData& texture);

        static void renderAll();
        static void onWindowChangeAll();
        // static void onLevelChangeAll(...);

        static void setRGB(Uint8 r, Uint8 g, Uint8 b);
        static void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        static void setAlpha(Uint8 alpha);
        static void setRGBA(SDL_Color col);

        SDL_Rect getDestRectFromCoords(const SDL_Point coords);

        static std::unordered_map<SDL_Point, T*, utils::hashers::SDL_Point_Hasher, utils::operators::SDL_Point_Equality_Operator> instanceMapping;
        static tiledata::AnimatedEntitiesTilesetData* tilesetData;

        SDL_Point destCoords;

    protected:
        BaseTextureWrapper();
        // Prevent the creation of a new instance of the class by copying an existing one
        BaseTextureWrapper(BaseTextureWrapper const&) = delete;
        // Prevent the assignment of one instance of the class to another
        BaseTextureWrapper& operator=(const BaseTextureWrapper&) = delete;

        SDL_Rect destRect;
        SDL_Rect srcRect;

        /**
         * @brief Modify `destRect`.
         * @param x,y Shift `destRect`'s coordinates by the corresponding value.
         * @param w,h Multiply `destRect`'s width/height by the value corresponded with that dimension. Also accordingly modify `destRect.x` and `destRect.y` so that the value of `center` does not change. Works separately with `destRectModifier.x` and `destRectModifier.y`.
         * @see BaseTextureWrapper.getDestRectFromCoords() (classmethod)
        */
        SDL_Rect destRectModifier;

        double angle = 0;
        SDL_Point center;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
};


/**
 * @brief An abstract class representing a texture that updates its animation.
*/
template <class T>
class AnimatedTextureWrapper : public BaseTextureWrapper<T> {
    public:
        using BaseTextureWrapper<T>::instanceMapping, BaseTextureWrapper<T>::tilesetData, BaseTextureWrapper<T>::destCoords, BaseTextureWrapper<T>::destRect, BaseTextureWrapper<T>::srcRect, BaseTextureWrapper<T>::destRectModifier, BaseTextureWrapper<T>::angle, BaseTextureWrapper<T>::center, BaseTextureWrapper<T>::flip;

        void updateAnimation();
        void resetAnimation(const tiledata::AnimatedEntitiesTilesetData::AnimationType animationType);

    protected:
        AnimatedTextureWrapper() = default;
        virtual ~AnimatedTextureWrapper() = default;

        tiledata::AnimatedEntitiesTilesetData::AnimationType currAnimationType;
        int currAnimationGID;
};


/**
 * @brief An abstract class representing a texture that updates its animation and is able to change its position.
*/
template <class T>
class AnimatedDynamicTextureWrapper : public AnimatedTextureWrapper<T> {
    public:
        using BaseTextureWrapper<T>::instanceMapping, BaseTextureWrapper<T>::tilesetData, BaseTextureWrapper<T>::destCoords, BaseTextureWrapper<T>::destRect, BaseTextureWrapper<T>::srcRect, BaseTextureWrapper<T>::destRectModifier, BaseTextureWrapper<T>::angle, BaseTextureWrapper<T>::center, BaseTextureWrapper<T>::flip;
        using AnimatedTextureWrapper<T>::currAnimationType, AnimatedTextureWrapper<T>::currAnimationGID;

        virtual ~AnimatedDynamicTextureWrapper();

        static void initialize(const std::filesystem::path xmlPath);
        void onLevelChange(const leveldata::TextureData& texture) override;

        virtual void move();
        virtual bool validateMove();

        void onMoveStart();
        void onMoveEnd();

        /**
         * @note Allow public access to encapsulated internal state.
        */
        bool isNextTileReached = false;

        SDL_Point* nextDestCoords = nullptr;
        SDL_Rect* nextDestRect = nullptr;

    protected:
        AnimatedDynamicTextureWrapper();

        SDL_Point velocity;   // between -1 and 1
        SDL_Point VELOCITY;
};


// typedef AnimatedDynamicTextureWrapper<Player>;
// typedef AnimatedTextureWrapper<Teleporter>;
// #include <../src/meta/base-texture.cpp>
// #include <../src/meta/animated-texture.cpp>
// #include <../src/meta/animated-dynamic-texture.cpp>