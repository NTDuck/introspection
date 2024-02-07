#include <components.hpp>

#include <SDL.h>


MenuAnimatedBackground::MenuAnimatedBackground(SDL_Texture*& texture) : pTexture(texture) {
    mSrcRects.first.x = mSrcRects.first.y = mSrcRects.second.y = mDestRects.first.y = mDestRects.second.x = mDestRects.second.y = 0;
}

/**
 * @brief Update `srcRects` and `destRects` based on `currAnimationUpdateCount`.
*/
void MenuAnimatedBackground::updateAnimation() {
    mCurrAnimationUpdateCount += kAnimationUpdateRate;
    if (mCurrAnimationUpdateCount > kAnimationUpdateRateLimit) mCurrAnimationUpdateCount = 0;

    mSrcRects.second.w = static_cast<int>(mCurrAnimationUpdateCount * mSrcSize.x);
    mSrcRects.first.w = mSrcRects.second.x = mSrcSize.x - mSrcRects.second.w;

    mDestRects.first.x = mDestRects.second.w = static_cast<int>(mCurrAnimationUpdateCount * mDestSize.x);
    mDestRects.first.w = globals::windowSize.x - mDestRects.first.x;
}

void MenuAnimatedBackground::render() const {
    SDL_RenderCopy(globals::renderer, pTexture, &mSrcRects.first, &mDestRects.first);
    SDL_RenderCopy(globals::renderer, pTexture, &mSrcRects.second, &mDestRects.second);
}

void MenuAnimatedBackground::onWindowChange() {
    SDL_QueryTexture(pTexture, nullptr, nullptr, &mSrcSize.x, &mSrcSize.y);
    mSrcRects.first.h = mSrcRects.second.h = mSrcSize.y;
    mDestRects.first.h = mDestRects.second.h = mDestSize.y;
}