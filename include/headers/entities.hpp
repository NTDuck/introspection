#ifndef ENTITIES_H
#define ENTITIES_H

#include <filesystem>
#include <functional>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_set>
#include <utility>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/* Abstract templates */

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
        INCL_MULTITON(T)

        virtual ~AbstractEntity() = default;

        static void initialize();
        static void deinitialize();

        template <typename LevelData>
        static void onLevelChangeAll(typename level::EntityLevelData::Collection<LevelData> const& entityLevelDataCollection);

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(level::EntityLevelData const& entityLevelData);

        virtual void handleCustomEventPOST() const {}
        virtual void handleCustomEventGET(SDL_Event const& event) {}

        static tile::EntitiesTilesetData* tilesetData;

        int id;

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
        AbstractEntity(SDL_Point const& destCoords);

        // Lifetime of a custom event, in order
        SDL_Event formatCustomEvent(int id_ = -1) const;

        template <typename Data>
        inline void populateCustomEvent(SDL_Event& event, Data const& data) const {
            if constexpr(std::is_same_v<Data, event::Code>) event.user.code = static_cast<Sint32>(data);
            else event.user.data1 = new Data(data);   // Implicit
        }

        template <typename Data>
        inline void populateCustomEvent(SDL_Event& event, event::Code code, Data const& data) const {
            populateCustomEvent(event, code);
            populateCustomEvent(event, data);
        }

        void enqueueCustomEvent(SDL_Event& event) const;

        SDL_Rect getDestRectFromCoords(SDL_Point const& coords) const;

        /**
         * Contain data associated with the position of the entity's sprite/animation relative to the tileset. In pixels.
        */
        SDL_Rect srcRect;

        static const std::filesystem::path kTilesetPath;

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

    private:
        static int idCounter;
};

namespace std {
    template <typename T>
    struct hash<AbstractEntity<T>> {
        std::size_t operator()(AbstractEntity<T> const*& instance) const;
    };

    template <typename T>
    struct equal_to<AbstractEntity<T>> {
        bool operator()(AbstractEntity<T> const*& first, AbstractEntity<T> const*& second) const;
    };
};

#define INCL_ABSTRACT_ENTITY(T) using AbstractEntity<T>::id, AbstractEntity<T>::initialize, AbstractEntity<T>::deinitialize, AbstractEntity<T>::onLevelChangeAll, AbstractEntity<T>::render, AbstractEntity<T>::onWindowChange, AbstractEntity<T>::onLevelChange, AbstractEntity<T>::handleCustomEventPOST, AbstractEntity<T>::handleCustomEventGET, AbstractEntity<T>::tilesetData, AbstractEntity<T>::destCoords, AbstractEntity<T>::destRect, AbstractEntity<T>::primaryStats, AbstractEntity<T>::secondaryStats, AbstractEntity<T>::getDestRectFromCoords, AbstractEntity<T>::formatCustomEvent, AbstractEntity<T>::populateCustomEvent, AbstractEntity<T>::enqueueCustomEvent, AbstractEntity<T>::kTilesetPath, AbstractEntity<T>::srcRect, AbstractEntity<T>::destRectModifier, AbstractEntity<T>::angle, AbstractEntity<T>::center, AbstractEntity<T>::flip;


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation.
*/
template <typename T>
class AbstractAnimatedEntity : public AbstractEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)

        virtual ~AbstractAnimatedEntity() = default;

        void onLevelChange(level::EntityLevelData const& entityLevelData) override;
        virtual void handleSFX() const;

        void initiateAnimation();
        virtual void updateAnimation();
        void resetAnimation(AnimationType animationType, EntityStatusFlag flag = EntityStatusFlag::kDefault);

        AnimationType currAnimationType;
        AnimationType* nextAnimationType = nullptr;
        bool isAnimationOnProgress = false;   // Works closely with `nextAnimationType` since they were once in a POD

        inline bool isAnimationAtSprite(int GID) const {
            return currAnimationGID == GID;
        }

        inline bool isAnimationAtFirstSprite() const {
            return isAnimationAtSprite(tilesetData->animationMapping[currAnimationType].startGID);
        }

        inline bool isAnimationAtFinalSprite() const {
            return isAnimationAtSprite(tilesetData->animationMapping[currAnimationType].stopGID);
        }

        /**
         * The minimum range required for the entity to initiate an attack on a targetable entity.
        */
        SDL_Point kAttackInitiateRange;

        /**
         * The minimum range required for the entity to register an attack on a targetable entity.
        */
        SDL_Point kAttackRegisterRange;

    protected:
        AbstractAnimatedEntity(SDL_Point const& destCoords);

    private:
        int currAnimationUpdateCount = 0;
        int currAnimationGID;
};

#define INCL_ABSTRACT_ANIMATED_ENTITY(T) using AbstractAnimatedEntity<T>::onLevelChange, AbstractAnimatedEntity<T>::handleSFX, AbstractAnimatedEntity<T>::initiateAnimation, AbstractAnimatedEntity<T>::updateAnimation, AbstractAnimatedEntity<T>::resetAnimation, AbstractAnimatedEntity<T>::currAnimationType, AbstractAnimatedEntity<T>::nextAnimationType, AbstractAnimatedEntity<T>::isAnimationOnProgress, AbstractAnimatedEntity<T>::isAnimationAtSprite, AbstractAnimatedEntity<T>::isAnimationAtFirstSprite, AbstractAnimatedEntity<T>::isAnimationAtFinalSprite, AbstractAnimatedEntity<T>::kAttackInitiateRange, AbstractAnimatedEntity<T>::kAttackRegisterRange;


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation and changes position.
*/
template <typename T>
class AbstractAnimatedDynamicEntity : public AbstractAnimatedEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)

        virtual ~AbstractAnimatedDynamicEntity();

        void onWindowChange() override;
        void onLevelChange(level::EntityLevelData const& entityLevelData) override;

        virtual void move();
        virtual void initiateMove(EntityStatusFlag flag = EntityStatusFlag::kDefault);

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
        AbstractAnimatedDynamicEntity(SDL_Point const& destCoords);

        virtual void onMoveStart(EntityStatusFlag flag = EntityStatusFlag::kDefault);
        virtual void onMoveEnd(EntityStatusFlag flag = EntityStatusFlag::kDefault);
        virtual void onRunningToggled(bool onRunningStart);

        virtual bool validateMove() const;

        /**
         * Represent the current direction of the entity.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point currVelocity;

        /**
         * Represent the multiplier applied to `kVelocity` should the entity switch to `kRun` animation.
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
        SDL_Point* nextVelocity = nullptr;

    private:
        void calculateVelocityDependencies();
        
        int counterMoveDelay;

        SDL_FPoint counterFractionalVelocity;
        SDL_FPoint kFractionalVelocity;
        SDL_Point kIntegralVelocity;
};

#define INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T) using AbstractAnimatedDynamicEntity<T>::onWindowChange, AbstractAnimatedDynamicEntity<T>::onLevelChange, AbstractAnimatedDynamicEntity<T>::move, AbstractAnimatedDynamicEntity<T>::initiateMove, AbstractAnimatedDynamicEntity<T>::validateMove, AbstractAnimatedDynamicEntity<T>::onMoveStart, AbstractAnimatedDynamicEntity<T>::onMoveEnd, AbstractAnimatedDynamicEntity<T>::onRunningToggled, AbstractAnimatedDynamicEntity<T>::isRunning, AbstractAnimatedDynamicEntity<T>::nextDestCoords, AbstractAnimatedDynamicEntity<T>::nextDestRect, AbstractAnimatedDynamicEntity<T>::currVelocity, AbstractAnimatedDynamicEntity<T>::runModifier, AbstractAnimatedDynamicEntity<T>::kMoveDelay, AbstractAnimatedDynamicEntity<T>::kVelocity, AbstractAnimatedDynamicEntity<T>::nextVelocity;


template <typename T>
class GenericTeleporterEntity : public AbstractAnimatedEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)

        virtual ~GenericTeleporterEntity() = default;
        
        void onLevelChange(level::EntityLevelData const& teleporterData) override;
        void handleCustomEventPOST() const override;

    protected:
        GenericTeleporterEntity(SDL_Point const& destCoords);

    private:
        void handleCustomEventPOST_kReq_Teleport_GTE_Player() const;

        /**
         * The new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
        */
        SDL_Point targetDestCoords;

        /**
         * The new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
        */
        level::LevelName targetLevel;
};

#define INCL_GENERIC_TELEPORTER_ENTITY(T) using GenericTeleporterEntity<T>::onLevelChange, GenericTeleporterEntity<T>::handleCustomEventPOST;


template <typename T>
class GenericHostileEntity : public AbstractAnimatedDynamicEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T)

        ~GenericHostileEntity() = default;

        void handleCustomEventPOST() const override;
        void handleCustomEventGET(SDL_Event const& event) override;

    protected:
        GenericHostileEntity(SDL_Point const& destCoords);

        /**
         * If the player entity is "within" the specified range, the slime entity would move towards the player. The slime would remain IDLE otherwise.
        */
        SDL_Point kMoveInitiateRange;

    private:
        void handleCustomEventPOST_kReq_AttackRegister_GHE_Player() const;
        void handleCustomEventPOST_kReq_AttackInitiate_GHE_Player() const;
        void handleCustomEventPOST_kReq_MoveInitiate_GHE_Player() const;

        void handleCustomEventGET_kReq_AttackRegister_Player_GHE(SDL_Event const& event);
        void handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_MoveTerminate_GHE_Player(SDL_Event const& event);
};

#define INCL_GENERIC_HOSTILE_ENTITY(T) using GenericHostileEntity<T>::handleCustomEventPOST, GenericHostileEntity<T>::handleCustomEventGET, GenericHostileEntity<T>::kMoveInitiateRange;


template <typename T>
class GenericSurgeProjectile : public AbstractAnimatedDynamicEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T)

        ~GenericSurgeProjectile() = default;

        static void onLevelChangeAll();
        void handleCustomEventPOST() const override;

        static void initiateAttack(ProjectileType type, SDL_Point const& destCoords, SDL_Point const& direction);

        void handleLifespan();

    protected:
        GenericSurgeProjectile(SDL_Point const& destCoords, SDL_Point const& direction);

    private:
        void initiateNextLinearAttack();
        void handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const;

        SDL_Point& kDirection = currVelocity;   // Does not allocate additional memory
};

#define INCL_GENERIC_SURGE_PROJECTILE(T) using GenericSurgeProjectile<T>::onLevelChangeAll, GenericSurgeProjectile<T>::handleCustomEventPOST, GenericSurgeProjectile<T>::initiateAttack, GenericSurgeProjectile<T>::handleLifespan;


template <typename T>
class GenericAerialProjectile : public AbstractAnimatedDynamicEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T)

        virtual ~GenericAerialProjectile() = default;

        static void onLevelChangeAll();
        static void initiateLinearAttack(SDL_Point const& destCoords, SDL_Point const& direction);

        void updateAnimation() override;

    protected:
        GenericAerialProjectile(SDL_Point const& destCoords, SDL_Point const& direction);

    private:
        void onMoveStart(EntityStatusFlag flag = EntityStatusFlag::kDefault) override;
        void onMoveEnd(EntityStatusFlag flag = EntityStatusFlag::kDefault) override; 
};

#define INCL_GENERIC_AERIAL_PROJECTILE(T) using GenericAerialProjectile<T>::onLevelChangeAll, GenericAerialProjectile<T>::initiateLinearAttack, GenericAerialProjectile<T>::updateAnimation;


/* Derived implementations */

class PentacleProjectile final : public GenericSurgeProjectile<PentacleProjectile> {
    public:
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(PentacleProjectile)
        INCL_GENERIC_SURGE_PROJECTILE(PentacleProjectile)

        PentacleProjectile(SDL_Point const& destCoords, SDL_Point const& direction);
        ~PentacleProjectile() = default;
};


class HauntedBookcaseProjectile final : public GenericAerialProjectile<HauntedBookcaseProjectile> {
    public:
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(HauntedBookcaseProjectile)
        INCL_GENERIC_AERIAL_PROJECTILE(HauntedBookcaseProjectile)

        HauntedBookcaseProjectile(SDL_Point const& destCoords, SDL_Point const& direction);
        ~HauntedBookcaseProjectile() = default;
};


/**
 * @brief A singleton class representing one instance of the player entity.
 * @note Only one instance should exist at a time.
*/
class Player final : public Singleton<Player>, public AbstractAnimatedDynamicEntity<Player> {
    public:
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(Player)
        INCL_SINGLETON(Player)

        Player(SDL_Point const& destCoords);
        ~Player() = default;

        static void deinitialize();

        void onLevelChange(level::EntityLevelData const& player) override;
        void handleKeyboardEvent(SDL_Event const& event);

        void handleCustomEventPOST() const override;
        void handleCustomEventGET(SDL_Event const& event) override;

    private:
        void handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const;
        void handleCustomEventPOST_kReq_Death_Player() const;

        void handleCustomEventGET_kReq_AttackRegister_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event);
};


/**
 * @brief A multiton class representing controlled instances of teleporter entities.
*/
class Teleporter final : public GenericTeleporterEntity<Teleporter> {
    public:
        INCL_ABSTRACT_ANIMATED_ENTITY(Teleporter)
        INCL_GENERIC_TELEPORTER_ENTITY(Teleporter)

        Teleporter(SDL_Point const& destCoords);
        ~Teleporter() = default;
};


/**
 * @brief A multiton class representing controlled instances of slime entities.
*/
class Slime final : public GenericHostileEntity<Slime> {
    public:
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(Slime)
        INCL_GENERIC_HOSTILE_ENTITY(Slime)

        Slime(SDL_Point const& destCoords);
        ~Slime() = default;
};


#endif