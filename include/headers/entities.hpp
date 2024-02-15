#ifndef ENTITIES_H
#define ENTITIES_H

#include <filesystem>
#include <functional>
#include <string>
#include <tuple>
#include <vector>
#include <queue>
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
        static void reinitialize(std::filesystem::path path);

        /**
         * @brief Clear `instanceMapping` then call `onLevelChange()` method on every instance of derived class `T`.
         * @todo Allow only `level::EntityLevelData` and its subclasses. Try `<type_traits>` and `<concepts>`.
        */
        static inline void onLevelChangeAll(std::vector<level::Data_Generic*> const& levelData) {
            Multiton<T>::deinitialize();
            sID_Counter = 0;

            for (const auto data : levelData) {
                if (data == nullptr) continue;
                auto instance = instantiate(data->destCoords);
                instance->onLevelChange(*data);
            }
        }

        virtual void render() const;
        virtual void onWindowChange();
        virtual void onLevelChange(level::Data_Generic const& entityLevelData);

        virtual void handleCustomEventPOST() const {}
        virtual void handleCustomEventGET(SDL_Event const& event) {}

    protected:
        AbstractEntity(SDL_Point const& destCoords);
        SDL_Rect getDestRectFromCoords(SDL_Point const& coords) const;

        static const char* sTypeID;
        const int mID;

        static std::filesystem::path sTilesetPath;
        static tile::EntitiesTilesetData sTilesetData;

        SDL_Point mDestCoords;
        SDL_Rect mSrcRect;
        SDL_Rect mDestRect;

        /**
         * Modify `mDestRect`.
         * @param x a value of `k` specifies that `destRect.x` would be shifted by `k * globals::tileDestSize.x` pixels.
         * @param y a value of `k` specifies that `destRect.y` would be shifted by `k * globals::tileDestSize.y` pixels.
         * @param w a value of `k` specifies that `destRect.w` would be multiplied by `k`.
         * @param h a value of `k` specifies that `destRect.h` would be multiplied by `k`.
         * @note The use of `float` might cause significant data loss.
         * @note Recommended implementation: this should not affect `center`.
        */
        SDL_FRect mDestRectModifier;

        double mAngle = 0;   // Clockwise rotation, in degrees
        SDL_Point* pCenter = nullptr;
        SDL_RendererFlip mFlip = SDL_FLIP_NONE;

        EntityPrimaryStats mPrimaryStats;
        EntitySecondaryStats mSecondaryStats;

    private:
        static int sID_Counter;
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

#define INCL_ABSTRACT_ENTITY(T) using AbstractEntity<T>::initialize, AbstractEntity<T>::deinitialize, AbstractEntity<T>::reinitialize, AbstractEntity<T>::onLevelChangeAll, AbstractEntity<T>::render, AbstractEntity<T>::onWindowChange, AbstractEntity<T>::onLevelChange, AbstractEntity<T>::handleCustomEventPOST, AbstractEntity<T>::handleCustomEventGET, AbstractEntity<T>::getDestRectFromCoords, AbstractEntity<T>::mID, AbstractEntity<T>::sTilesetPath, AbstractEntity<T>::sTilesetData, AbstractEntity<T>::mDestCoords, AbstractEntity<T>::mSrcRect, AbstractEntity<T>::mDestRect, AbstractEntity<T>::mDestRectModifier, AbstractEntity<T>::mAngle, AbstractEntity<T>::pCenter, AbstractEntity<T>::mFlip, AbstractEntity<T>::mPrimaryStats, AbstractEntity<T>::mSecondaryStats;


/**
 * @brief An abstract class combining CRTP and adapted Multiton pattern. Represents an entity that updates animation.
*/
template <typename T>
class AbstractAnimatedEntity : public AbstractEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)

        virtual ~AbstractAnimatedEntity() = default;

        void onLevelChange(level::Data_Generic const& entityLevelData) override;
        virtual void handleSFX() const;

        void initiateAnimation();
        virtual void updateAnimation();
        void resetAnimation(AnimationType animationType, EntityStatusFlag flag = EntityStatusFlag::kDefault);

        inline bool isAnimationAtSprite(int GID) const {
            return mCurrAnimationGID == GID;
        }

        inline bool isAnimationAtFirstSprite() const {
            return isAnimationAtSprite(sTilesetData.animationMapping[mCurrAnimationType].startGID);
        }

        inline bool isAnimationAtFinalSprite() const {
            return isAnimationAtSprite(sTilesetData.animationMapping[mCurrAnimationType].stopGID);
        }

    protected:
        AbstractAnimatedEntity(SDL_Point const& destCoords);

        AnimationType mCurrAnimationType;
        AnimationType* pNextAnimationType = nullptr;
        bool mIsAnimationOnProgress = false;   // Works closely with `nextAnimationType` since they were once in a POD

        SDL_Point mAttackRegisterRange;

    private:
        int mCurrAnimationUpdateCount = 0;
        int mCurrAnimationGID;
};

#define INCL_ABSTRACT_ANIMATED_ENTITY(T) using AbstractAnimatedEntity<T>::onLevelChange, AbstractAnimatedEntity<T>::handleSFX, AbstractAnimatedEntity<T>::initiateAnimation, AbstractAnimatedEntity<T>::updateAnimation, AbstractAnimatedEntity<T>::resetAnimation, AbstractAnimatedEntity<T>::isAnimationAtSprite, AbstractAnimatedEntity<T>::isAnimationAtFirstSprite, AbstractAnimatedEntity<T>::isAnimationAtFinalSprite, AbstractAnimatedEntity<T>::mCurrAnimationType, AbstractAnimatedEntity<T>::pNextAnimationType, AbstractAnimatedEntity<T>::mIsAnimationOnProgress, AbstractAnimatedEntity<T>::mAttackRegisterRange;

/**
 * @brief A shorthand to declare a AAE-derived. Used in header files only.
 * @note Does not include explicit template instantiation.
*/
#define DECLARE_ABSTRACT_ANIMATED_ENTITY(T) \
class T final : public AbstractAnimatedEntity<T> {\
    public:\
        INCL_ABSTRACT_ANIMATED_ENTITY(T)\
        \
        T(SDL_Point const& destCoords);\
        ~T() = default;\
};

/**
 * @brief A shorthand to define a AAE-derived. Used in source files only.
 * @note Does not include dependencies i.e. `#include ...`
*/
#define DEFINE_ABSTRACT_ANIMATED_ENTITY(T, ns) \
T::T(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {\
    mDestRectModifier = ns::destRectModifier;\
}\
\
template <>\
const char* AbstractEntity<T>::sTypeID = ns::typeID;\
\
template <>\
std::filesystem::path AbstractEntity<T>::sTilesetPath = ns::path;\


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
        void onLevelChange(level::Data_Generic const& entityLevelData) override;

        virtual void move();
        virtual void initiateMove(EntityStatusFlag flag = EntityStatusFlag::kDefault);

    protected:
        AbstractAnimatedDynamicEntity(SDL_Point const& destCoords);

        virtual void onMoveStart(EntityStatusFlag flag = EntityStatusFlag::kDefault);
        virtual void onMoveEnd(EntityStatusFlag flag = EntityStatusFlag::kDefault);
        virtual void onRunningToggled(bool onRunningStart);

        virtual bool validateMove() const;

        bool mIsRunning = false;

        /**
         * A pointer to the "next" `mDestCoords`.
         * @note Recommended implementation: this member should be set when the entity "moves" (implemented by derived classes). Upon successful validation, its stored value should be reassigned to `destCoords`. This member should then be nullified regardless.
        */
        SDL_Point* pNextDestCoords = nullptr;

        /**
         * A pointer to the "next" `mDestRect`.
         * @note Recommended implementation: this member should be used in strict conjunction with `nextDestCoords`.
        */
        SDL_Rect* pNextDestRect = nullptr;

        /**
         * Represent the multiplier applied to `sVelocity` should the entity switch to `kRun` animation.
        */
        static const double sRunModifier;

        /**
         * Represent the minimum number of frames of inaction, between 2 consecutive moves.
        */
        static int sMoveDelay;

        /**
         * For dimension `i`, a number of `kVelocity.i` frames is required for the entity to make a full move i.e. `destRect.i` shifts by `globals::tileDestSize.i` pixels.
         * @note This constant and the "physical" speed of the entity are in an inverse ratio.
        */
        static SDL_FPoint sVelocity;
        
        /**
         * Represent the current direction of the entity.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point mCurrVelocity;

        /**
         * Represent the next direction of the entity. Might change every frame.
         * @note Data members should only receive values of `-1`, `1`, and `0`.
        */
        SDL_Point* pNextVelocity = nullptr;

    private:
        void calculateVelocityDependencies();
        
        int mMoveDelayCounter;

        SDL_FPoint mFractionalVelocityCounter;
        SDL_FPoint mFractionalVelocity;
        SDL_Point mIntegralVelocity;
};

#define INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T) using AbstractAnimatedDynamicEntity<T>::onWindowChange, AbstractAnimatedDynamicEntity<T>::onLevelChange, AbstractAnimatedDynamicEntity<T>::move, AbstractAnimatedDynamicEntity<T>::initiateMove, AbstractAnimatedDynamicEntity<T>::onMoveStart, AbstractAnimatedDynamicEntity<T>::onMoveEnd, AbstractAnimatedDynamicEntity<T>::onRunningToggled, AbstractAnimatedDynamicEntity<T>::validateMove, AbstractAnimatedDynamicEntity<T>::mIsRunning, AbstractAnimatedDynamicEntity<T>::pNextDestCoords, AbstractAnimatedDynamicEntity<T>::pNextDestRect, AbstractAnimatedDynamicEntity<T>::sRunModifier, AbstractAnimatedDynamicEntity<T>::sMoveDelay, AbstractAnimatedDynamicEntity<T>::sVelocity, AbstractAnimatedDynamicEntity<T>::mCurrVelocity, AbstractAnimatedDynamicEntity<T>::pNextVelocity;


template <typename T>
class GenericTeleporterEntity : public AbstractAnimatedEntity<T> {
    public:
        INCL_MULTITON(T)
        INCL_ABSTRACT_ENTITY(T)
        INCL_ABSTRACT_ANIMATED_ENTITY(T)

        virtual ~GenericTeleporterEntity() = default;
        
        void onLevelChange(level::Data_Generic const& teleporterData) override;
        void handleCustomEventPOST() const override;

    protected:
        GenericTeleporterEntity(SDL_Point const& destCoords);

    private:
        void handleCustomEventPOST_kReq_Teleport_GTE_Player() const;

        /**
         * The new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
        */
        SDL_Point mTargetDestCoords;

        /**
         * The new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
        */
        level::Name mTargetLevel;
};

#define INCL_GENERIC_TELEPORTER_ENTITY(T) using GenericTeleporterEntity<T>::onLevelChange, GenericTeleporterEntity<T>::handleCustomEventPOST;

#define DECLARE_GENERIC_TELEPORTER_ENTITY(T) \
class T final : public GenericTeleporterEntity<T> {\
    public:\
        INCL_ABSTRACT_ANIMATED_ENTITY(T)\
        INCL_GENERIC_TELEPORTER_ENTITY(T)\
        \
        T(SDL_Point const& destCoords);\
        ~T() = default;\
};

#define DEFINE_GENERIC_TELEPORTER_ENTITY(T, ns) \
T::T(SDL_Point const& destCoords) : GenericTeleporterEntity<T>(destCoords) {\
    mDestRectModifier = ns::destRectModifier;\
}\
\
template <>\
const char* AbstractEntity<T>::sTypeID = ns::typeID;\
\
template <>\
std::filesystem::path AbstractEntity<T>::sTilesetPath = ns::path;


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
        SDL_Point mMoveInitiateRange;
        SDL_Point mAttackInitiateRange;

    private:
        void handleCustomEventPOST_kReq_AttackRegister_GHE_Player() const;
        void handleCustomEventPOST_kReq_AttackInitiate_GHE_Player() const;
        void handleCustomEventPOST_kReq_MoveInitiate_GHE_Player() const;

        void handleCustomEventGET_kReq_AttackRegister_Player_GHE(SDL_Event const& event);
        void handleCustomEventGET_kResp_AttackInitiate_GHE_Player();
        void handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_MoveTerminate_GHE_Player();
};

#define INCL_GENERIC_HOSTILE_ENTITY(T) using GenericHostileEntity<T>::handleCustomEventPOST, GenericHostileEntity<T>::handleCustomEventGET, GenericHostileEntity<T>::mMoveInitiateRange, GenericHostileEntity<T>::mAttackInitiateRange;

#define DECLARE_GENERIC_HOSTILE_ENTITY(T) \
class T final : public GenericHostileEntity<T> {\
    public:\
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T)\
        INCL_GENERIC_HOSTILE_ENTITY(T)\
        \
        T(SDL_Point const& destCoords);\
        ~T() = default;\
};

#define DEFINE_GENERIC_HOSTILE_ENTITY(T, ns) \
T::T(SDL_Point const& destCoords) : GenericHostileEntity<T>(destCoords) {\
    mDestRectModifier = ns::destRectModifier;\
    mMoveInitiateRange = ns::moveInitiateRange;\
    mAttackInitiateRange = ns::attackInitiateRange;\
    mAttackRegisterRange = ns::attackRegisterRange;\
    mPrimaryStats = ns::primaryStats;\
}\
\
template <>\
int AbstractAnimatedDynamicEntity<T>::sMoveDelay = ns::moveDelay;\
\
template <>\
SDL_FPoint AbstractAnimatedDynamicEntity<T>::sVelocity = ns::velocity;\
\
template <>\
const char* AbstractEntity<T>::sTypeID = ns::typeID;\
\
template <>\
std::filesystem::path AbstractEntity<T>::sTilesetPath = ns::path;\


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

        void handleInstantiation();
        static void handleTermination();

    protected:
        GenericSurgeProjectile(SDL_Point const& destCoords, SDL_Point const& direction);

    private:
        void initiateNextLinearAttack();
        void handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const;

        SDL_Point& mDirection = mCurrVelocity;   // Does not allocate additional memory

        static std::queue<T*> sTerminatedInstances;
};

#define INCL_GENERIC_SURGE_PROJECTILE(T) using GenericSurgeProjectile<T>::onLevelChangeAll, GenericSurgeProjectile<T>::handleCustomEventPOST, GenericSurgeProjectile<T>::initiateAttack, GenericSurgeProjectile<T>::handleInstantiation, GenericSurgeProjectile<T>::handleTermination;

/**
 * @note Second parameter `direction` of constructor is provided with a decoy value to prevent a specific compilation error.
*/
#define DECLARE_GENERIC_SURGE_PROJECTILE(T) \
class T final : public GenericSurgeProjectile<T> {\
    public:\
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(T)\
        INCL_GENERIC_SURGE_PROJECTILE(T)\
        \
        T(SDL_Point const& destCoords, SDL_Point const& direction = {});\
        ~T() = default;\
};

#define DEFINE_GENERIC_SURGE_PROJECTILE(T, ns) \
T::T(SDL_Point const& destCoords, SDL_Point const& direction) : GenericSurgeProjectile<T>(destCoords, direction) {\
    mDestRectModifier = ns::destRectModifier;\
    mAttackRegisterRange = ns::attackRegisterRange;\
    mPrimaryStats = ns::primaryStats;\
    onWindowChange();\
}\
\
template <>\
int AbstractAnimatedDynamicEntity<T>::sMoveDelay = ns::moveDelay;\
\
template <>\
SDL_FPoint AbstractAnimatedDynamicEntity<T>::sVelocity = ns::velocity;\
\
template <>\
const char* AbstractEntity<T>::sTypeID = ns::typeID;\
\
template <>\
std::filesystem::path AbstractEntity<T>::sTilesetPath = ns::path;


/* Derived implementations */

class Player final : public Singleton<Player>, public AbstractAnimatedDynamicEntity<Player> {
    public:
        INCL_ABSTRACT_ANIMATED_DYNAMIC_ENTITY(Player)
        INCL_SINGLETON(Player)

        Player(SDL_Point const& destCoords);
        ~Player() = default;

        static void deinitialize();
        static void reinitialize(bool increment);

        void onLevelChange(level::Data_Generic const& player) override;
        void handleKeyboardEvent(SDL_Event const& event);
        void handleMouseEvent(SDL_Event const& event);

        void handleCustomEventPOST() const override;
        void handleCustomEventGET(SDL_Event const& event) override;

    private:
        void handleKeyboardEvent_Movement(SDL_Event const& event);
        void handleKeyboardEvent_ProjectileAttack(SDL_Event const& event);

        void handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const;
        void handleCustomEventPOST_kReq_Death_Player() const;

        void handleCustomEventGET_kReq_AttackRegister_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event);
        void handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event);

        static const std::vector<std::filesystem::path> sTilesetPaths;

        SDL_Point mPrevDirection = { 1, 0 };
};


DECLARE_GENERIC_SURGE_PROJECTILE(PentacleProjectile)

DECLARE_GENERIC_TELEPORTER_ENTITY(Teleporter)
DECLARE_GENERIC_TELEPORTER_ENTITY(RedHandThroneTeleporter)

DECLARE_GENERIC_HOSTILE_ENTITY(Slime)

DECLARE_ABSTRACT_ANIMATED_ENTITY(OmoriLaptop)
DECLARE_ABSTRACT_ANIMATED_ENTITY(OmoriLightBulb)
DECLARE_ABSTRACT_ANIMATED_ENTITY(OmoriMewO)


#endif