#include <filesystem>
#include <sstream>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


AnimatedTextureWrapper::AnimatedTextureWrapper() {}

AnimatedTextureWrapper::~AnimatedTextureWrapper() {
    BaseTextureWrapper::~BaseTextureWrapper();
}

void AnimatedTextureWrapper::init_(const std::filesystem::path xmlPath) {
    BaseTextureWrapper::init_(xmlPath);

    for (const auto& pair : properties) {
        std::istringstream iss(pair.second);
        std::pair<int, int> GIDs;
        iss >> GIDs.first >> GIDs.second;
        rotatingGIDs[pair.first] = GIDs;
    }

    // This is really ugly and needs immediate correction
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(xmlPath.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    srcRectCount.x = document.child("tileset").attribute("columns").as_int();
    srcRectCount.y = document.child("tileset").attribute("tilecount").as_int() / srcRectCount.x;

    currAnimationState = "animation-walk";
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

    srcRect.x = currAnimationGID % srcRectCount.x * srcRect.w;
    srcRect.y = currAnimationGID / srcRectCount.x * srcRect.h;
}

/**
 * @brief Switch to new animation state i.e. new collection of sprites.
*/
void AnimatedTextureWrapper::resetAnimation(const std::string nextAnimationState) {
    currAnimationState = nextAnimationState;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}