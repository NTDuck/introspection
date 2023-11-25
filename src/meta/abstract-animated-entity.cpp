#include <meta.hpp>

#include <filesystem>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity() {
    resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kIdle);
}

/**
 * @brief Call `updateAnimation()` method on every instance of derived class `T`.
*/
template <class T>
void AbstractAnimatedEntity<T>::updateAnimationAll() {
    for (auto& pair : instanceMapping) pair.second->updateAnimation();
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <class T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    static int animationUpdateCount = 0;
    ++animationUpdateCount;
    
    if (animationUpdateCount == tilesetData->animationUpdateRate) {
        animationUpdateCount = 0;
        if (currAnimationGID < tilesetData->animationMapping[currAnimationType].stopGID) {
            currAnimationGID += tilesetData->animationSize.x;
            // Behold, heresy!
            if (currAnimationGID / tilesetData->animationSize.x != (currAnimationGID - tilesetData->animationSize.x) / tilesetData->animationSize.x) currAnimationGID += tilesetData->srcCount.x * (tilesetData->animationSize.y - 1);
        } else {
            currAnimationGID = tilesetData->animationMapping[currAnimationType].startGID;
        };
    }

    srcRect.x = currAnimationGID % tilesetData->srcCount.x * tilesetData->srcSize.x;
    srcRect.y = currAnimationGID / tilesetData->srcCount.x * tilesetData->srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <class T>
void AbstractAnimatedEntity<T>::resetAnimation(const tile::AnimatedEntitiesTilesetData::AnimationType animationType) {
    currAnimationType = animationType;
    currAnimationGID = AbstractEntity<T>::tilesetData->animationMapping[currAnimationType].startGID;
}


template class AbstractAnimatedEntity<Player>;
template class AbstractAnimatedEntity<Teleporter>;