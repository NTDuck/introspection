#include <components.hpp>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries.hpp>


MenuParallax::MenuParallax() {
    mTexture = IMG_LoadTexture(globals::renderer, config::components::menu_parallax::path.generic_string().c_str());
    SDL_QueryTexture(mTexture, nullptr, nullptr, &mSrcSize.x, &mSrcSize.y);

    mSrcRects.first.x = mSrcRects.first.y = mSrcRects.second.y = mDestRects.first.y = mDestRects.second.x = mDestRects.second.y = 0;
}

MenuParallax::~MenuParallax() {
    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
}

/**
 * @brief Update `srcRects` and `destRects` based on `currAnimationUpdateCount`.
*/
void MenuParallax::updateAnimation() {
    mCurrAnimationUpdateCount += kAnimationUpdateRate;
    if (mCurrAnimationUpdateCount > kAnimationUpdateRateLimit) mCurrAnimationUpdateCount = 0;

    mSrcRects.second.w = static_cast<int>(mCurrAnimationUpdateCount * mSrcSize.x);
    mSrcRects.first.w = mSrcRects.second.x = mSrcSize.x - mSrcRects.second.w;

    mDestRects.first.x = mDestRects.second.w = static_cast<int>(mCurrAnimationUpdateCount * mDestSize.x);
    mDestRects.first.w = globals::windowSize.x - mDestRects.first.x;
}

void MenuParallax::render() const {
    SDL_RenderCopy(globals::renderer, mTexture, &mSrcRects.first, &mDestRects.first);
    SDL_RenderCopy(globals::renderer, mTexture, &mSrcRects.second, &mDestRects.second);
}

void MenuParallax::onWindowChange() {
    mSrcRects.first.h = mSrcRects.second.h = mSrcSize.y;
    mDestRects.first.h = mDestRects.second.h = mDestSize.y;
}