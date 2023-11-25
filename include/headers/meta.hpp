#ifndef META_H
#define META_H

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents a generic entity.
 * @note Provide flexibility and compile-time polymorphism.
 * @see https://stackoverflow.com/questions/26950274/implementing-crtp-and-issue-with-undefined-reference
 * @see https://google.github.io/styleguide/cppguide.html#Declaration_Order
*/
template <class T>
class AbstractEntity {
    public:
        static T* instantiate(SDL_Point destCoords);
        AbstractEntity(AbstractEntity const&) = delete;   // copy constructor
        AbstractEntity& operator=(const AbstractEntity&) = delete;   // copy assignment constructor
        AbstractEntity(AbstractEntity&&) = delete;   // move constructor
        AbstractEntity& operator=(AbstractEntity&&) = delete;   // move assignment constructor
        virtual ~AbstractEntity();

        static void initialize(const std::filesystem::path& xmlPath);
        static void deinitialize();

        static void setRGB(Uint8 r, Uint8 g, Uint8 b);
        static void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        static void setAlpha(Uint8 alpha);
        static void setRGBA(SDL_Color& color);

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(const level::EntityLevelData& textureData);

        SDL_Rect getDestRectFromCoords(const SDL_Point& coords);

        static std::unordered_map<SDL_Point, T*, utils::hashers::SDL_Point_Hasher, utils::operators::SDL_Point_Equality_Operator> instanceMapping;
        static tile::AnimatedEntitiesTilesetData* tilesetData;

        /**
         * The non-negative coordinates of the entity relative to a 2-dimensional standardized Cartesian coordinate system. (origin at top-left corner)
         * @see <utils.h> tile::TileCollection
        */
        SDL_Point destCoords;

    protected:
        AbstractEntity();

        /**
         * Contain data associated with the position of the entity relative to the window. In pixels.
        */
        SDL_Rect destRect;

        /**
         * Contain data associated with the position of the entity's sprite/animation relative to the tileset. In pixels.
        */
        SDL_Rect srcRect;

        /**
         * Modify `destRect`.
         * @param x,y Shift `destRect`'s coordinates by the corresponding value. In pixels.
         * @param w,h Multiply `destRect`'s width/height by the value corresponded with that dimension. Also accordingly modify `destRect.x` and `destRect.y` so that the value of `center` does not change. Works separately with `destRectModifier.x` and `destRectModifier.y`.
        */
        SDL_Rect destRectModifier;

        /**
         * Indicate the rotation (clockwise) applied to the entity. In degrees.
        */
        double angle;

        /**
         * A pointer to the point relative to the window, around which the entity would be rotated.
        */
        SDL_Point* center;

        /**
         * An `SDL_RendererFlip` enumeration constant indicating the flipping method to be used on the entity.
         * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
        */
        SDL_RendererFlip flip;
};


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation.
*/
template <class T>
class AbstractAnimatedEntity : public AbstractEntity<T> {
    public:
        using AbstractEntity<T>::instanceMapping, AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;

        virtual ~AbstractAnimatedEntity() = default;

        void updateAnimation();
        void resetAnimation(const tile::AnimatedEntitiesTilesetData::AnimationType animationType);

        tile::AnimatedEntitiesTilesetData::AnimationType currAnimationType;

    protected:
        AbstractAnimatedEntity();

    private:
        int currAnimationGID;
};


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation and changes position.
*/
template <class T>
class AbstractAnimatedDynamicEntity : public AbstractAnimatedEntity<T> {
    public:
        using AbstractEntity<T>::instanceMapping, AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;
        using AbstractAnimatedEntity<T>::currAnimationType;

        virtual ~AbstractAnimatedDynamicEntity();

        void onLevelChange(const level::EntityLevelData& textureData) override;

        virtual void move();
        virtual bool validateMove();
        void onMoveStart();
        void onMoveEnd();

        bool isNextTileReached = false;   // Allow public access to encapsulated internal state.

        /**
         * A pointer to the "next" `destCoords`.
         * @note Recommended implementation: this member should be set when the entity "moves" (implemented by derived classes). Upon successful validation, its stored value should be reassigned to `destCoords`. This member should then be nullified regardless.
        */
        SDL_Point* nextDestCoords = nullptr;

        /**
         * A pointer to the "next" `destRect`.
         * @note Recommended implementation: this member should be used in strict conjunction with `nextDestCoords`.
        */
        SDL_Rect* nextDestRect = nullptr;

    protected:
        AbstractAnimatedDynamicEntity();

        /**
         * Represent the number of pixels the entity should shift per frame, during movement.
        */
        const SDL_Point kVelocity;

        /**
         * Represent the current direction of the entity.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point currentVelocity;
};


#endif