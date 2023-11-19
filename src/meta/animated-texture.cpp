#include <filesystem>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


AnimatedTextureWrapper::AnimatedTextureWrapper() {}

AnimatedTextureWrapper::~AnimatedTextureWrapper() { BaseTextureWrapper::~BaseTextureWrapper(); }

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <src/interface.cpp> Interface.renderLevel() (classmethod)
*/
void AnimatedTextureWrapper::updateAnimation() {
    static int animationUpdateCount = 0;
    ++animationUpdateCount;

    if (animationUpdateCount == tilesetData.animationUpdateRate) {
        animationUpdateCount = 0;
        if (currAnimationGID == tilesetData.animationMapping[currAnimationType].stopGID) currAnimationGID = tilesetData.animationMapping[currAnimationType].startGID; else {
            currAnimationGID += tilesetData.animationSize.x;
            // The following calculations bear fruition directly from the dev's brain, and should be understood as-is. Additional explanation will not be provided elsewhere.
            if (currAnimationGID / tilesetData.animationSize.x != (currAnimationGID - tilesetData.animationSize.x) / tilesetData.animationSize.x) currAnimationGID += tilesetData.srcCount.x * (tilesetData.animationSize.y - 1);
        };
    }

    srcRect.x = currAnimationGID % tilesetData.srcCount.x * srcRect.w * tilesetData.animationSize.x;
    srcRect.y = currAnimationGID / tilesetData.srcCount.x * srcRect.h * tilesetData.animationSize.y;
}

/**
 * @brief Switch to new animation state i.e. new collection of sprites.
*/
void AnimatedTextureWrapper::resetAnimation(const tiledata::AnimatedEntitiesTilesetData::AnimationType animationType) {
    currAnimationType = animationType;
    currAnimationGID = tilesetData.animationMapping[currAnimationType].startGID;
}