#include <entities.hpp>

#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Flow 1: Initialization of `nextAnimationData`
 * ┌─────────────┐   ┌───────────────────────────┐
 * │ interaction ├───┤ checkEntityAnimation(...) ├───┐
 * └─────────────┘   └───────────────────────────┘   │
 *                                                   │
 * ┌─────────────┐   ┌────────────────────────┐      │
 * │    Game     ├───┤ onEntityAnimation(...) ◄──────┘
 * └─────────────┘   └────────────────────────┘
*/

/**
 * @brief Flow 2: Retrieval & Modification of `nextAnimationData`
 * ┌────────────────────────┐
 * │ AbstractAnimatedEntity │
 * └───┬────────────────────┘
 *     │
 *     │   ┌─────────────────────┐
 *     ├───┤ initiateAnimation() ├────────┐
 *     │   └─────────────────────┘        │
 *     │                                  │
 *     │       ┌──── conditional check ◄──┘
 *     │       │
 *     │       └───► priority overlap ────┬───┐
 *     │                                  │   │
 *     │       ┌──── isExecuting = false ◄┘   │
 *     │       │                              │
 *     │   ┌───▼───────────┐                  │
 *     └───┤ onAnimation() ◄──────────────────┘
 *         └───┬───────────┘
 *             │
 *             └───► isExecuting = true
*/

/**
 * @note There is no Flow 3. For Deallocation of `nextAnimationData`, see `updateAnimation()`.
*/

template <typename T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity(SDL_Point const& destCoords) : AbstractEntity<T>(destCoords) {
    resetAnimation(AnimationType::kIdle);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractAnimatedEntity<T>::onLevelChange(level::EntityLevelData const& entityLevelData) {
    if (nextAnimationType != nullptr) {
        delete nextAnimationType;
        nextAnimationType = nullptr;
        isAnimationOnProgress = false;
    }
    
    AbstractEntity<T>::onLevelChange(entityLevelData);
}

/**
 * @note `AnimationType::kWalk` & `AnimationType:kRun` are handled elsewhere.
 * @see https://stackoverflow.com/questions/41011900/equivalent-ternary-operator-for-constexpr-if
*/
template <typename T>
void AbstractAnimatedEntity<T>::handleSFX() const {
    switch (currAnimationType) {
        case AnimationType::kAttack:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerAttack : Mixer::SFXName::kEntityAttack);
            break;

        case AnimationType::kDamaged:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kEntityDamaged);
            break;

        case AnimationType::kDeath:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerDeath : Mixer::SFXName::kEntityDeath);
            break;

        default: break;
    }
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <typename T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    ++currAnimationUpdateCount;
    
    if (currAnimationUpdateCount >= static_cast<int>(tilesetData->animationUpdateRate * tilesetData->animationMapping[currAnimationType].animationUpdateRateMultiplier)) {
        currAnimationUpdateCount = 0;
        if (currAnimationGID < tilesetData->animationMapping[currAnimationType].stopGID) {
            currAnimationGID += tilesetData->animationSize.x;
            if (currAnimationGID / tilesetData->srcCount.x != (currAnimationGID - tilesetData->srcCount.x) / tilesetData->srcCount.x) currAnimationGID += tilesetData->srcCount.x * (tilesetData->animationSize.y - 1);   // Behold, heresy!
        } else {
            // Deinitialize `nextAnimationData`
            if (nextAnimationType != nullptr && isAnimationOnProgress) {
                delete nextAnimationType;
                nextAnimationType = nullptr;
                isAnimationOnProgress = false;
            }

            if (currAnimationType == AnimationType::kDeath) return;   // The real permanent
            resetAnimation(tilesetData->animationMapping[currAnimationType].isPermanent ? AnimationType::kIdle : currAnimationType);
        };
    }

    srcRect.x = currAnimationGID % tilesetData->srcCount.x * tilesetData->srcSize.x;
    srcRect.y = currAnimationGID / tilesetData->srcCount.x * tilesetData->srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <typename T>
void AbstractAnimatedEntity<T>::resetAnimation(AnimationType animationType, EntityStatusFlag flag) {
    currAnimationType = animationType;
    if (flag == EntityStatusFlag::kContinued) return;
    currAnimationGID = AbstractEntity<T>::tilesetData->animationMapping[currAnimationType].startGID;
}

/**
 * @brief Initiate a new animation based on `nextAnimationData`.
*/
template <typename T>
void AbstractAnimatedEntity<T>::initiateAnimation() {
    // Check for priority overlap
    if (currAnimationType == AnimationType::kDeath) return;
    if (nextAnimationType == nullptr || isAnimationOnProgress) return;

    resetAnimation(*nextAnimationType);
    isAnimationOnProgress = true;
}

// /**
//  * @note `event.user.data1` is relevant data, `event.user.data2` is id
// */
// template <typename T>
// void AbstractAnimatedEntity<T>::handleCustomEventPOST() {
//     if (currAnimationType == AnimationType::kAttack && isAnimationAtFinalSprite()) {
//         event.user.code = static_cast<Sint32>(std::is_same_v<T, Player> || std::is_same_v<T, PentacleProjectile> ? event::Code::kRequestPlayerAttackRegister : event::Code::kRequestEntityAttackRegister);
//         event.user.data1 = new event::EntityAttackData({ destCoords, kAttackRegisterRange, secondaryStats });   // implicit
//         auto event = formatCustomEvent();
//         populateCustomEvent(event, )
//     } else if constexpr(std::is_same_v<T, Player>) {
//         event.user.code = static_cast<Sint32>(event::Code::kPlayerGeneric);
//         event.user.data1 = new event::PlayerGenericData({ destCoords, currAnimationType });
//     }
// }

// template <typename T>
// void AbstractAnimatedEntity<T>::handleCustomEventGET(SDL_Event const& event) {
//     auto handleDamaged = [&](SDL_Event const& event) {
//         auto data = *reinterpret_cast<event::EntityAttackData*>(event.user.data1);   // Careful when dealing with `void*`

//         // Check
//         if (currAnimationType == AnimationType::kDamaged) return;
//         int distance = utils::calculateDistance(destCoords, data.destCoords);
//         if (distance > data.attackRegisterRange.x || distance > data.attackRegisterRange.y) return;

//         // Execute
//         secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(data.stats, secondaryStats);
//         secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedMagicDamage(data.stats, secondaryStats);

//         if (nextAnimationType == nullptr) {
//             nextAnimationType = new AnimationType(secondaryStats.HP > 0 ? AnimationType::kDamaged : AnimationType::kDeath);
//             isAnimationOnProgress = false;
//         }
//     };

//     switch (static_cast<event::Code>(event.user.code)) {
//         case event::Code::kRequestPlayerAttackRegister:
//             if constexpr(std::is_same_v<T, Player> || std::is_same_v<T, PentacleProjectile>) break;
//             handleDamaged(event); break;
        
//         case event::Code::kRequestEntityAttackRegister:
//             if constexpr(!(std::is_same_v<T, Player> || std::is_same_v<T, PentacleProjectile>)) break;
//             handleDamaged(event); break;

//         default: break;
//     }
// }


template class AbstractAnimatedEntity<PentacleProjectile>;
template class AbstractAnimatedEntity<HauntedBookcaseProjectile>;
template class AbstractAnimatedEntity<Player>;
template class AbstractAnimatedEntity<Teleporter>;
template class AbstractAnimatedEntity<Slime>;