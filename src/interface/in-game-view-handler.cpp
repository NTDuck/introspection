#include <interface.hpp>

#include <cmath>
#include <functional>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameViewHandler::IngameViewHandler(std::function<void()> const& callable, SDL_Rect& targetedEntityDestRect) : AbstractInterface<IngameViewHandler>(), kRenderMethod(callable), mTargetedEntityDestRect(targetedEntityDestRect) {}

void IngameViewHandler::render() const {
    // Focus on player entity
    SDL_SetRenderTarget(globals::renderer, mTexture);

    // Render dependencies
    std::invoke(kRenderMethod);

    switch (mView) {
        case View::kFullScreen:
            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, mTexture, nullptr, &mDestRect);
            break;

        case View::kTargetEntity:
            // Calculate rendered portion
            mViewport.x = mTargetedEntityDestRect.x + mTargetedEntityDestRect.w / 2 - mViewport.w / 2;
            mViewport.y = mTargetedEntityDestRect.y + mTargetedEntityDestRect.h / 2 - mViewport.h / 2;

            // "Fix" out-of-bound cases
            if (mViewport.x < 0) mViewport.x = 0;
            else if (mViewport.x + mViewport.w > mTextureSize.x) mViewport.x = mTextureSize.x - mViewport.w;
            if (mViewport.y < 0) mViewport.y = 0;
            else if (mViewport.y + mViewport.h > mTextureSize.y) mViewport.y = mTextureSize.y - mViewport.h;

            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, mTexture, &mViewport, nullptr);
            break;
    }
}

void IngameViewHandler::onWindowChange() {
    mTileCountWidth = static_cast<double>(globals::windowSize.x) / static_cast<double>(globals::windowSize.y) * mTileCountHeight;   // `mTileCountHeight` is immutable

    mViewport.w = mTileCountWidth * level::data.tileDestSize.x;
    mViewport.h = mTileCountHeight * level::data.tileDestSize.y;

    switch (mView) {
        case View::kFullScreen:
            // mTileDestSize = 1 << static_cast<int>(log2(std::min(globals::windowSize.x / globals::tileDestCount.x, globals::windowSize.y / globals::tileDestCount.y)));
            mTileDestSize.x = mTileDestSize.y = std::min(globals::windowSize.x / level::data.tileDestCount.x, globals::windowSize.y / level::data.tileDestCount.y);   // Sacrifice absolute "powers of 2" for decreased offset
            break;

        case View::kTargetEntity:
            mTileDestSize = level::data.tileDestSize;
            break;
    }

    mDestRect.w = level::data.tileDestCount.x * mTileDestSize.x;
    mDestRect.h = level::data.tileDestCount.y * mTileDestSize.y;
    mDestRect.x = (globals::windowSize.x - mDestRect.w) / 2;
    mDestRect.y = (globals::windowSize.y - mDestRect.h) / 2;

}

void IngameViewHandler::onLevelChange() {
    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTextureSize = {
        level::data.tileDestCount.x * level::data.tileDestSize.x,
        level::data.tileDestCount.y * level::data.tileDestSize.y,
    };
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextureSize.x, mTextureSize.y);
    
    onWindowChange();
}

/**
 * @see https://stackoverflow.com/questions/12761315/random-element-from-unordered-set-in-o1
*/
void IngameViewHandler::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case config::key::INGAME_TOGGLE_CAMERA_ANGLE:
            if (event.type != SDL_KEYDOWN) break;
            switchView();
            break;

        default: break;
    }
}

void IngameViewHandler::switchView() {
    mView = static_cast<View>(!static_cast<bool>(mView));
    onWindowChange();
}