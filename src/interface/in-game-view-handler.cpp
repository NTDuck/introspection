#include <interface.hpp>

#include <functional>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameViewHandler::IngameViewHandler(std::function<void()> const& callable, SDL_Rect& destRect, const IngameViewMode viewMode) : AbstractInterface<IngameViewHandler>(), kRenderMethod(callable), mViewMode(viewMode), mDestRect(destRect) {}

void IngameViewHandler::render() const {
    switch (mViewMode) {
        case IngameViewMode::kFullScreen:
            std::invoke(kRenderMethod);
            break;

        case IngameViewMode::kFocusOnEntity:
            // Focus on player entity
            SDL_SetRenderTarget(globals::renderer, mTexture);

            // Render dependencies
            std::invoke(kRenderMethod);

            // Calculate rendered portion
            mSrcRect.x = mDestRect.x + mDestRect.w / 2 - mSrcRect.w / 2;
            mSrcRect.y = mDestRect.y + mDestRect.h / 2 - mSrcRect.h / 2;

            // "Fix" out-of-bound cases
            if (mSrcRect.x < 0) mSrcRect.x = 0;
            else if (mSrcRect.x + mSrcRect.w > globals::windowSize.x) mSrcRect.x = globals::windowSize.x - mSrcRect.w;
            if (mSrcRect.y < 0) mSrcRect.y = 0;
            else if (mSrcRect.y + mSrcRect.h > globals::windowSize.y) mSrcRect.y = globals::windowSize.y - mSrcRect.h;

            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, mTexture, &mSrcRect, nullptr);
            break;
    }
}

void IngameViewHandler::onWindowChange() {
    AbstractInterface<IngameViewHandler>::onWindowChange();
    
    mTileCountWidth = static_cast<double>(globals::windowSize.x) / static_cast<double>(globals::windowSize.y) * mTileCountHeight;
    mSrcRect.w = mTileCountWidth * globals::tileDestSize.x;
    mSrcRect.h = mTileCountHeight * globals::tileDestSize.y;
}

/**
 * @see https://stackoverflow.com/questions/12761315/random-element-from-unordered-set-in-o1
*/
void IngameViewHandler::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case config::key::INGAME_TOGGLE_CAMERA_ANGLE:
            if (event.type != SDL_KEYDOWN) break;
            switchViewMode(mViewMode == IngameViewMode::kFocusOnEntity ? IngameViewMode::kFullScreen : IngameViewMode::kFocusOnEntity);
            break;

        default: break;
    }
}

void IngameViewHandler::switchViewMode(const IngameViewMode newViewMode) {
    if (mViewMode == newViewMode) return;
    mViewMode = newViewMode;
}