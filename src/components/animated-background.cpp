#include <components.hpp>

#include <SDL.h>


AnimatedBackground::AnimatedBackground(SDL_Texture*& texture) : texture(texture) {
    srcRects.first.x = srcRects.first.y = srcRects.second.y = destRects.first.y = destRects.second.x = destRects.second.y = 0;
}

void AnimatedBackground::updateAnimation() {
    currAnimationUpdateCount += kAnimationUpdateRate;
    if (currAnimationUpdateCount > animationUpdateRateLimit) currAnimationUpdateCount = 0;

    srcRects.second.w = static_cast<int>(currAnimationUpdateCount * srcSize.x);
    srcRects.first.w = srcRects.second.x = srcSize.x - srcRects.second.w;

    destRects.first.x = destRects.second.w = static_cast<int>(currAnimationUpdateCount * destSize.x);
    destRects.first.w = globals::windowSize.x - destRects.first.x;
}

void AnimatedBackground::render() const {
    SDL_RenderCopy(globals::renderer, texture, &srcRects.first, &destRects.first);
    SDL_RenderCopy(globals::renderer, texture, &srcRects.second, &destRects.second);
}

void AnimatedBackground::onWindowChange() {
    SDL_QueryTexture(texture, nullptr, nullptr, &srcSize.x, &srcSize.y);
    srcRects.first.h = srcRects.second.h = srcSize.y;
    destRects.first.h = destRects.second.h = destSize.y;
}