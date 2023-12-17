#ifndef META_H
#define META_H

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility> 
#include <iostream>

#include <SDL.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Flow 1: Abstract Hierarchy
 * ┌────────────────────┐
 * │ PolymorphicBase<T> │
 * └───┬────────────────┘
 *     │                        ┌─────────────────┐
 *     │   ┌──────────────┐  ┌──► IngameInterface │
 *     ├───► Singleton<T> ├──┤  └─────────────────┘
 *     │   └──────────────┘  │
 *     │                     └───────────────────────────┐
 *     │   ┌─────────────┐                               │
 *     └───► Multiton<T> │                               │
 *         └───┬─────────┘                               │
 *             │                                         │
 *             │                                         │
 * ┌───────────▼───────┐                                 │
 * │ AbstractEntity<T> │                                 │
 * └───┬───────────────┘                                 │
 *     │                                                 │
 * ┌───▼───────────────────────┐   ┌────────────┐        │
 * │ AbstractAnimatedEntity<T> ├───► Teleporter │        │
 * └───┬───────────────────────┘   └────────────┘        │
 *     │                                                 │
 *     │                                    ┌────────┐   │
 *     │                                 ┌──► Player ◄───┘
 * ┌───▼──────────────────────────────┐  │  └────────┘
 * │ AbstractAnimatedDynamicEntity<T> ├──┤
 * └──────────────────────────────────┘  │  ┌───────┐
 *                                       └──► Slime │
 *                                          └───────┘
*/


/**
 * @brief An abstract base with certain operators deleted. Required for implementation of derived classes `Singleton<T>` and `Multiton<T>`.
*/
template <class T>
class PolymorphicBase {
    public:
        PolymorphicBase(PolymorphicBase const&) = delete;
        PolymorphicBase& operator=(PolymorphicBase const&) = delete;
        PolymorphicBase(PolymorphicBase&&) = delete;
        PolymorphicBase& operator=(PolymorphicBase&&) = delete;

    protected:
        PolymorphicBase() = default;
        virtual ~PolymorphicBase() = default;
};

/**
 * @brief A standard Singleton template class.
*/
template <class T>
class Singleton : virtual public PolymorphicBase<T> {
    public:
        template <typename... Args>
        inline static T* instantiate(Args&&... args) {
            if (instance == nullptr) instance = new T(std::forward<Args>(args)...);
            return instance;
        }

    protected:
        static T* instance;
};

/**
 * @brief An adapted Multiton template class that governs instances via a `std::unordered_set` instead of a `std::unordered_map`.
*/
template <class T>
class Multiton : virtual public PolymorphicBase<T> {
    struct Hasher {
        std::size_t operator()(const T* pointer) const;
    };

    struct EqualityOperator {
        bool operator()(const T* first, const T* second) const;
    };

    public:
        template <typename... Args>
        inline static T* instantiate(Args&&... args) {
            auto instance = new T(std::forward<Args>(args)...);
            instances.emplace(instance);
            return instance;
        }

        static std::unordered_set<T*, Hasher, EqualityOperator> instances;
};


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents a generic entity.
 * @note Provide flexibility and compile-time polymorphism.
 * @note Recommended implementation: derived classes are responsible for the following:
 * 1. Define of static member `std::filesystem::path tilesetPath`
 * 2. Place constructor at public scope
 * @see https://stackoverflow.com/questions/4173254/what-is-the-curiously-recurring-template-pattern-crtp
 * @see https://stackoverflow.com/questions/26950274/implementing-crtp-and-issue-with-undefined-reference
 * @see https://google.github.io/styleguide/cppguide.html#Declaration_Order
*/
template <class T>
class AbstractEntity : public Multiton<T> {
    public:
        using Multiton<T>::instances;

        static T* instantiate(SDL_Point destCoords);
        
        bool operator==(const AbstractEntity<T>& other) const;
        bool operator<(const AbstractEntity<T>& other) const;
        virtual ~AbstractEntity();

        static void initialize();
        static void deinitialize();

        static void setRGB(Uint8 r, Uint8 g, Uint8 b);
        static void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
        static void setAlpha(Uint8 alpha);
        static void setRGBA(SDL_Color& color);

        /**
         * @brief Variadically call `method` on each instance of derived class `T` with the same parameters `Args...`.
         * @note `inline` could solve GCC's `undefined reference`. Sometimes.
        */
        template <typename ClassMethod, typename... Args>
        inline static void callOnEach(ClassMethod&& method, Args&&... args) {
            for (auto& instance : instances) (instance->*method)(std::forward<Args>(args)...);
        }

        template <typename LevelData> static void callOnEach_onLevelChange(const typename level::EntityLevelData::Collection<LevelData>& entityLevelDataCollection);

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(const level::EntityLevelData& entityLevelData);

        SDL_Rect getDestRectFromCoords(const SDL_Point& coords);

        static tile::AnimatedEntitiesTilesetData* tilesetData;

        /**
         * The non-negative coordinates of the entity relative to a 2-dimensional standardized Cartesian coordinate system. (origin at top-left corner)
         * @see <utils.h> tile::TileCollection
        */
        SDL_Point destCoords;

        /**
         * Contain data associated with the position of the entity relative to the window. In pixels.
        */
        SDL_Rect destRect;

        EntityPrimaryStats primaryStats;
        EntitySecondaryStats secondaryStats;
        
    protected:
        AbstractEntity();

        static const std::filesystem::path kTilesetPath;

        /**
         * Contain data associated with the position of the entity's sprite/animation relative to the tileset. In pixels.
        */
        SDL_Rect srcRect;

        /**
         * Modify `destRect`.
         * @param x a value of `k` specifies that `destRect.x` would be shifted by `k * globals::tileDestSize.x` pixels.
         * @param y a value of `k` specifies that `destRect.y` would be shifted by `k * globals::tileDestSize.y` pixels.
         * @param w a value of `k` specifies that `destRect.w` would be multiplied by `k`.
         * @param h a value of `k` specifies that `destRect.h` would be multiplied by `k`.
         * @note The use of `float` might cause significant data loss.
         * @note Recommended implementation: this should not affect `center`.
        */
        SDL_FRect destRectModifier;

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
        using Multiton<T>::instances;
        using AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::primaryStats, AbstractEntity<T>::secondaryStats, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;

        virtual ~AbstractAnimatedEntity() = default;
        void onLevelChange(const level::EntityLevelData& entityLevelData) override;

        void updateAnimation();
        void resetAnimation(const AnimationType animationType, const MoveStatusFlag flag = MoveStatusFlag::kDefault);

        virtual void initiateAnimation();
        virtual void onAttackInitiated();
        virtual void onAttackRegistered();
        virtual void onDeath();

        AnimationType currAnimationType;
        bool isAnimationAtFinalSprite;

        /**
         * The minimum range required for the entity to initiate an attack on a targetable entity.
        */
        SDL_Point kAttackInitiateRange;

        /**
         * The minimum range required for the entity to register an attack on a targetable entity.
        */
        SDL_Point kAttackRegisterRange;

        /**
         * Contain data associated with the pending i.e. "next" animation.
        */
        tile::NextAnimationData* nextAnimationData = nullptr;

    protected:
        AbstractAnimatedEntity();

    private:
        int currAnimationUpdateCount = 0;
        int currAnimationGID;
};


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation and changes position.
*/
template <class T>
class AbstractAnimatedDynamicEntity : public AbstractAnimatedEntity<T> {
    public:
        using Multiton<T>::instances;
        using AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::primaryStats, AbstractEntity<T>::secondaryStats, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;
        using AbstractAnimatedEntity<T>::currAnimationType, AbstractAnimatedEntity<T>::isAnimationAtFinalSprite, AbstractAnimatedEntity<T>::kAttackInitiateRange, AbstractAnimatedEntity<T>::kAttackRegisterRange, AbstractAnimatedEntity<T>::nextAnimationData;

        virtual ~AbstractAnimatedDynamicEntity();

        void onWindowChange() override;
        void onLevelChange(const level::EntityLevelData& entityLevelData) override;

        virtual void move();
        virtual void initiateMove(const MoveStatusFlag flag = MoveStatusFlag::kDefault);
        virtual bool validateMove();
        virtual void onMoveStart(const MoveStatusFlag flag = MoveStatusFlag::kDefault);
        virtual void onMoveEnd(const MoveStatusFlag flag = MoveStatusFlag::kDefault);
        virtual void onRunningToggled(const bool onRunningStart);

        bool isRunning = false;

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
        AbstractAnimatedDynamicEntity() = default;

        /**
         * Represent the multiplier applied to `kVelocity` should the entity switch to `kRunning` animation.
        */
        static const double runModifier;

        /**
         * Represent the minimum number of frames of inaction, between 2 consecutive moves.
        */
        static int kMoveDelay;

        /**
         * For dimension `i`, a number of `kVelocity.i` frames is required for the entity to make a full move i.e. `destRect.i` shifts by `globals::tileDestSize.i` pixels.
         * @note This constant and the "physical" speed of the entity are in an inverse ratio.
        */
        static SDL_FPoint kVelocity;

        /**
         * Represent the next direction of the entity. Might change every frame.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point nextVelocity;

    private:
        void calculateVelocityDependencies();
        
        int counterMoveDelay;

        /**
         * Represent the current direction of the entity.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point currVelocity;

        SDL_FPoint counterFractionalVelocity;
        SDL_FPoint kFractionalVelocity;
        SDL_Point kIntegralVelocity;
};


#endif