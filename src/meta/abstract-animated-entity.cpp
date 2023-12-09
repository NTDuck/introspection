#include <meta.hpp>

#include <filesystem>

#include <SDL.h>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


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

template <class T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity() : isAnimationAtFinalSprite(false) {
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kIdle);
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <class T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    ++currAnimationUpdateCount;
    
    if (currAnimationUpdateCount == tilesetData->animationUpdateRate) {
        currAnimationUpdateCount = 0;
        if (currAnimationGID < tilesetData->animationMapping[currAnimationType].stopGID) {
            currAnimationGID += tilesetData->animationSize.x;
            // Behold, heresy!
            if (currAnimationGID / tilesetData->animationSize.x != (currAnimationGID - tilesetData->animationSize.x) / tilesetData->animationSize.x) currAnimationGID += tilesetData->srcCount.x * (tilesetData->animationSize.y - 1);
        } else {
            // Deinitialize `nextAnimationData`
            if (nextAnimationData != nullptr && nextAnimationData->isExecuting) {
                delete nextAnimationData;
                nextAnimationData = nullptr;
            }

            if (tilesetData->animationMapping[currAnimationType].isPermanent) {
                if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDeath) return;   // The real permanent
                resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kIdle);   // Might implement a temporary storage for most recent state to switch back to
            }
            currAnimationGID = tilesetData->animationMapping[currAnimationType].startGID;
        };
    }

    isAnimationAtFinalSprite = (currAnimationGID == tilesetData->animationMapping[currAnimationType].stopGID);

    srcRect.x = currAnimationGID % tilesetData->srcCount.x * tilesetData->srcSize.x;
    srcRect.y = currAnimationGID / tilesetData->srcCount.x * tilesetData->srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <class T>
void AbstractAnimatedEntity<T>::resetAnimation(const tile::AnimatedEntitiesTilesetData::AnimationType animationType, const MoveStatusFlag flag) {
    currAnimationType = animationType;
    if (flag == MoveStatusFlag::kContinued) return;
    currAnimationGID = AbstractEntity<T>::tilesetData->animationMapping[currAnimationType].startGID;
}


/**
 * @brief Initiate a new animation based on `nextAnimationData`.
*/
template <class T>
void AbstractAnimatedEntity<T>::initiateAnimation() {
    // Check for priority overlap
    if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDeath) return;
    if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kAttack && nextAnimationData != nullptr && nextAnimationData->animationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged) onAttackRegistered();

    if (nextAnimationData == nullptr || nextAnimationData->isExecuting) return;

    switch (nextAnimationData->animationType) {
        case tile::AnimatedEntitiesTilesetData::AnimationType::kAttack: onAttackInitiated(); break;
        case tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged: onAttackRegistered(); break;
        case tile::AnimatedEntitiesTilesetData::AnimationType::kDeath: onDeath(); break;
        default: return;
    }

    nextAnimationData->isExecuting = true;
}

/**
 * @brief Called when the entity should inititate an attack.
*/
template <class T>
void AbstractAnimatedEntity<T>::onAttackInitiated() {
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kAttack);
}

/**
 * @brief Called when the entity should be damaged.
*/
template <class T>
void AbstractAnimatedEntity<T>::onAttackRegistered() {
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged);
}

/**
 * @brief Called when the entity's `secondaryStats.HP` should drop below zero.
*/
template <class T>
void AbstractAnimatedEntity<T>::onDeath() {
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kDeath);
}

template class AbstractAnimatedEntity<Player>;
template class AbstractAnimatedEntity<Teleporter>;
template class AbstractAnimatedEntity<Slime>;