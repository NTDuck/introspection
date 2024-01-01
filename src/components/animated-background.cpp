#include <components.hpp>

#include <SDL.h>


AnimatedBackground::AnimatedBackground(SDL_Texture*& texture, SDL_Point const& srcSize) : texture(texture), srcSize(srcSize) {}

void AnimatedBackground::updateAnimation() {
    currAnimationUpdateCount += kAnimationUpdateRate;
    if (currAnimationUpdateCount > animationUpdateRateLimit) currAnimationUpdateCount = 0;
}

void AnimatedBackground::render() const {
    const SDL_Rect firstSrcRectPortion = { 0, 0, static_cast<int>((1 - currAnimationUpdateCount) * srcSize.x), srcSize.y };
    const SDL_Rect secondSrcRectPortion = { static_cast<int>((1 - currAnimationUpdateCount) * srcSize.x), 0, static_cast<int>(currAnimationUpdateCount * srcSize.x), srcSize.y };
    const SDL_Rect firstDestRectPortion = { static_cast<int>(currAnimationUpdateCount * globals::windowSize.x), 0, static_cast<int>((1 - currAnimationUpdateCount) * globals::windowSize.x), globals::windowSize.y };
    const SDL_Rect secondDestRectPortion = { 0, 0, static_cast<int>(currAnimationUpdateCount * globals::windowSize.x), globals::windowSize.y };

    SDL_RenderCopy(globals::renderer, texture, &firstSrcRectPortion, &firstDestRectPortion);
    SDL_RenderCopy(globals::renderer, texture, &secondSrcRectPortion, &secondDestRectPortion);
}

void AnimatedBackground::onWindowChange(SDL_Point const& nextSrcSize) {
    srcSize = nextSrcSize;
}