#include <interface.hpp>

#include <functional>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameViewHandler::IngameViewHandler(std::function<void()> const& callable, SDL_Rect& destRect, const IngameViewMode viewMode) : AbstractInterface<IngameViewHandler>(), kRenderMethod(callable), mViewMode(viewMode), mDestRect(destRect) {}

void IngameViewHandler::render() const {
    // Focus on player entity
    SDL_SetRenderTarget(globals::renderer, mTexture);

    // Render dependencies
    std::invoke(kRenderMethod);

    switch (mViewMode) {
        case IngameViewMode::kFullScreen:
            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, mTexture, nullptr, nullptr);
            // Calculate `destRect` here: PENDING
            break;

        case IngameViewMode::kFocusOnEntity:
            // Calculate rendered portion
            mSrcRect.x = mDestRect.x + mDestRect.w / 2 - mSrcRect.w / 2;
            mSrcRect.y = mDestRect.y + mDestRect.h / 2 - mSrcRect.h / 2;

            // "Fix" out-of-bound cases
            if (mSrcRect.x < 0) mSrcRect.x = 0;
            else if (mSrcRect.x + mSrcRect.w > mTextureSize.x) mSrcRect.x = mTextureSize.x - mSrcRect.w;
            if (mSrcRect.y < 0) mSrcRect.y = 0;
            else if (mSrcRect.y + mSrcRect.h > mTextureSize.y) mSrcRect.y = mTextureSize.y - mSrcRect.h;

            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, mTexture, &mSrcRect, nullptr);
            break;
    }
}

void IngameViewHandler::onWindowChange() {
    mTileCountWidth = static_cast<double>(globals::windowSize.x) / static_cast<double>(globals::windowSize.y) * mTileCountHeight;
    mSrcRect.w = mTileCountWidth * globals::tileDestSize.x;
    mSrcRect.h = mTileCountHeight * globals::tileDestSize.y;
}

void IngameViewHandler::onLevelChange() {
    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTextureSize = {
        globals::tileDestCount.x * globals::tileDestSize.x,
        globals::tileDestCount.y * globals::tileDestSize.y,
    };
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextureSize.x, mTextureSize.y);
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