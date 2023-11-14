#include <filesystem>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


AnimatedTextureWrapper::AnimatedTextureWrapper() {}

AnimatedTextureWrapper::~AnimatedTextureWrapper() { BaseTextureWrapper::~BaseTextureWrapper(); }

/**
 * @note Involves Tiled custom type `animation` that contains 3 attributes: `startGID` (`-1` by default), `stopGID` (`-1` by default), 'isPermanent` (`false` by default).
*/
void AnimatedTextureWrapper::init_(const std::filesystem::path xmlPath) {
    BaseTextureWrapper::init_(xmlPath);

    // Register to `rotatingGID`
    for (const auto& pair : tilesetData.propertiesEx) {
        auto startGID = pair.second.find("startGID");
        auto stopGID = pair.second.find("stopGID");

        if (startGID == pair.second.end() || stopGID == pair.second.end()) continue;

        rotatingGIDs.insert(std::make_pair(pair.first, std::make_pair(std::stoi(startGID -> second), std::stoi(stopGID -> second))));
    }

    currAnimationState = globals::config::PLAYER_ANIMATION_DEFAULT;
    animationUpdateRate = globals::config::ANIMATED_TEXTURE_ANIMATION_UPDATE_RATE;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <src/interface.cpp> Interface.renderLevel() (classmethod)
*/
void AnimatedTextureWrapper::updateAnimation() {
    static int animationUpdateCount = 0;
    ++animationUpdateCount;

    if (animationUpdateCount == animationUpdateRate) {
        animationUpdateCount = 0;
        if (currAnimationGID == rotatingGIDs[currAnimationState].second) currAnimationGID = rotatingGIDs[currAnimationState].first; else ++currAnimationGID;
    }

    srcRect.x = currAnimationGID % tilesetData.srcCount.x * srcRect.w;
    srcRect.y = currAnimationGID / tilesetData.srcCount.x * srcRect.h;
}

/**
 * @brief Switch to new animation state i.e. new collection of sprites.
*/
void AnimatedTextureWrapper::resetAnimation(const std::string nextAnimationState) {
    currAnimationState = nextAnimationState;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}