#ifndef META_H
#define META_H

#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility> 

#include <SDL.h>

#include <auxiliaries.hpp>


/**
 * @brief Flow 1: Abstract Hierarchy
 * ┌────────────────────┐
 * │ PolymorphicBase<T> │
 * └┬───────────────────┘
 *  │
 *  │ ┌──────────────┐
 *  ├─► Singleton<T> ├───────────────────────────────────────────┐
 *  │ └┬─────────────┘                                           │
 *  │  │                                                         │
 *  │  │ ┌──────────────────────┐ ┌─────────────────┐            │
 *  │  └─► AbstractInterface<T> ├─► IngameInterface │            │
 *  │    └──────────────────────┘ └─────────────────┘            │
 *  │                                                            │
 *  │ ┌─────────────┐                                            │
 *  └─► Multiton<T> │                                            │
 *    └┬────────────┘                                            │
 *     │                                                         │
 *     │ ┌───────────────────┐                                   │
 *     └─► AbstractEntity<T> │                                   │
 *       └┬──────────────────┘                                   │
 *        │                                                      │
 *        │ ┌───────────────────────────┐ ┌────────────┐         │
 *        └─► AbstractAnimatedEntity<T> ├─► Teleporter │         │
 *          └┬──────────────────────────┘ └────────────┘         │
 *           │                                                   │
 *           │ ┌──────────────────────────────────┐   ┌────────┐ │
 *           └─► AbstractAnimatedDynamicEntity<T> ├─┬─► Player ◄─┘
 *             └──────────────────────────────────┘ │ └────────┘
 *                                                  │
 *                                                  │ ┌───────┐
 *                                                  └─► Slime │
 *                                                    └───────┘
*/


/* Abstract-Abstract Template Classes */

/**
 * @brief An abstract base with certain operators deleted. Required for implementation of derived classes `Singleton<T>` and `Multiton<T>`.
*/
template <typename T>
class PolymorphicBase {
    public:
        PolymorphicBase(PolymorphicBase const&) = delete;
        PolymorphicBase& operator=(PolymorphicBase const&) = delete;
        PolymorphicBase(PolymorphicBase&&) = delete;
        PolymorphicBase& operator=(PolymorphicBase&&) = delete;

    protected:
        template <typename... Args>
        inline PolymorphicBase(Args&&... args) {}
        virtual ~PolymorphicBase() = default;
};

/**
 * @brief A standard Singleton template class.
*/
template <typename T>
class Singleton : virtual public PolymorphicBase<T> {
    public:
        template <typename... Args>
        static T* instantiate(Args&&... args) {
            if (instance == nullptr) instance = new T(std::forward<Args>(args)...);
            return instance;
        }

        static void deinitialize() {
            delete instance;
            instance = nullptr;
        }

    protected:
        static T* instance;
};

/**
 * @brief An adapted Multiton template class that governs instances via a `std::unordered_set` instead of a `std::unordered_map`.
*/
template <typename T>
class Multiton : virtual public PolymorphicBase<T> {
    public:      
        template <typename... Args>
        static T* instantiate(Args&&... args) {
            auto instance = new T(std::forward<Args>(args)...);
            instances.emplace(instance);
            return instance;
        }

        // template <typename... Args>
        // static void instantiateEx(Args&&... args) {
        //     const std::size_t size = std::min({args.size()...});
        //     for (std::size_t ind = 0; ind < size; ++ind) T::instantiate(args[ind]...);
        // }

        static void deinitialize() {
            // Somehow this yields weird segfaults. Consider switching to smart pointers?
            // Also, why is this read-only?
            // for (auto& instance : instances) if (instance) utils::dealloc(instance);
            instances.clear();
        }

        /**
         * @brief Variadically call `method` on each instance of derived class `T` with the same parameters `args`.
         * @note Defined here to avoid lengthy explicit template instantiation.
        */
        template <typename Callable, typename... Args>
        static void callOnEach(Callable&& callable, Args&&... args) {
            for (auto& instance : instances) std::invoke(std::forward<Callable>(callable), *instance, std::forward<Args>(args)...);
        }

        // /**
        //  * @brief Variadically call `method` on each instance of derived class `T` with parameters deduced from `args`.
        //  * @note Recommended implementation: `instances.size()` must be equivalent to the length of each container in `args`; any exceptions to which might result in undefined behavior. For example, with `method` `func(int, char)` and `args` being `std::vector<int>{0, 1, 2}, std::string("foo")`, given that `instances` currently houses `first`, `second`, `third`, this function will yield `first.func(0, 'f')`, `second.func(1, 'o')`, `third.func(2, 'o')`.
        // */
        // template <typename ClassMethod, typename... Args>
        // static void callOnEachEx(ClassMethod&& method, Args&&... args) {
        //     const std::size_t size = std::min({args.size()...});
        //     auto it = instances.begin();
        //     for (std::size_t ind = 0; ind < size && it != instances.end(); ++ind, ++it) (*it->*method)(args[ind]...);   // `std::forward` fails - requires a reference to the iterable, not the iterated element
        // }

        static std::unordered_set<T*> instances;

    protected:
        virtual ~Multiton() override {
            instances.erase(static_cast<T*>(this));   // remove from `instances`
        }
};

/* Internal initialization of static members */
template <typename T>
T* Singleton<T>::instance = nullptr;

template <typename T>
std::unordered_set<T*> Multiton<T>::instances;


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
template <typename T>
class AbstractEntity : public Multiton<T> {
    public:
        using Multiton<T>::instances, Multiton<T>::callOnEach;

        static T* instantiate(SDL_Point destCoords);

        static void initialize();
        static void deinitialize();

        template <typename LevelData>
        static void callOnEach_onLevelChange(typename level::EntityLevelData::Collection<LevelData> const& entityLevelDataCollection);

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(level::EntityLevelData const& entityLevelData);

        static tile::EntitiesTilesetData* tilesetData;

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
        SDL_Rect getDestRectFromCoords(SDL_Point const& coords) const;

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
        double angle = 0;

        /**
         * A pointer to the point relative to the window, around which the entity would be rotated.
        */
        SDL_Point* center = nullptr;

        /**
         * An `SDL_RendererFlip` enumeration constant indicating the flipping method to be used on the entity.
         * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
        */
        SDL_RendererFlip flip = SDL_FLIP_NONE;
};

namespace std {
    template <typename T>
    struct hash<AbstractEntity<T>> {
        std::size_t operator()(AbstractEntity<T> const*& instance) const {
            return instance == nullptr ? std::hash<std::nullptr_t>{}(instance) : std::hash<SDL_Point>(instance->destCoords);
        }
    };

    template <typename T>
    struct equal_to<AbstractEntity<T>> {
        bool operator()(AbstractEntity<T> const*& first, AbstractEntity<T> const*& second) const {
            return (first == nullptr && second == nullptr) || (first && second && first->destCoords == second->destCoords);
        }
    };
};

/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation.
*/
template <typename T>
class AbstractAnimatedEntity : public AbstractEntity<T> {
    public:
        using Multiton<T>::instances, Multiton<T>::callOnEach;
        using AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::primaryStats, AbstractEntity<T>::secondaryStats, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;

        virtual ~AbstractAnimatedEntity() = default;
        void onLevelChange(level::EntityLevelData const& entityLevelData) override;

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
template <typename T>
class AbstractAnimatedDynamicEntity : public AbstractAnimatedEntity<T> {
    public:
        using Multiton<T>::instances, Multiton<T>::callOnEach;
        using AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::primaryStats, AbstractEntity<T>::secondaryStats, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;
        using AbstractAnimatedEntity<T>::currAnimationType, AbstractAnimatedEntity<T>::isAnimationAtFinalSprite, AbstractAnimatedEntity<T>::kAttackInitiateRange, AbstractAnimatedEntity<T>::kAttackRegisterRange, AbstractAnimatedEntity<T>::nextAnimationData;

        virtual ~AbstractAnimatedDynamicEntity();

        void onWindowChange() override;
        void onLevelChange(level::EntityLevelData const& entityLevelData) override;

        virtual void move();
        virtual void initiateMove(const MoveStatusFlag flag = MoveStatusFlag::kDefault);
        virtual bool validateMove() const;
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


template <typename T>
class AbstractInterface : public Singleton<T> {
    friend Singleton<T>;
    public:
        using Singleton<T>::instantiate, Singleton<T>::deinitialize, Singleton<T>::instance;

        virtual void render() const;
        virtual void onWindowChange();

    protected:
        ~AbstractInterface();

        /**
         * @brief A temporary storage that is rendered every frame. Used to prevent multiple unnecessary calls of `SDL_RenderCopy()`.
         * @note Needs optimization to perfect relative positions of props to entities.
        */
        SDL_Texture* texture = nullptr;
};


#endif