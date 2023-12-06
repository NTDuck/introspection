#include <meta.hpp>

#include <filesystem>
#include <type_traits>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


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
            // Reset
            delete nextAnimationData;
            nextAnimationData = nullptr;

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
 * @brief Called when the entity should inititate an attack.
*/
template <class T>
void AbstractAnimatedEntity<T>::initiateAttack() {
    if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kAttack) return;
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kAttack);
}

/**
 * @brief Called when the entity is damaged.
*/
template <class T>
void AbstractAnimatedEntity<T>::onDamaged() {
    if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged) return;
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged);
}


template class AbstractAnimatedEntity<Player>;
template class AbstractAnimatedEntity<Teleporter>;
template class AbstractAnimatedEntity<Slime>;