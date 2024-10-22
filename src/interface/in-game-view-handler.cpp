#include <interface.hpp>

#include <cmath>
#include <functional>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameViewHandler::IngameViewHandler(std::function<void()> const& callable, SDL_Rect& targetedEntityDestRect) : AbstractInterface<IngameViewHandler>(), kRenderMethod(callable), mTargetedEntityDestRect(targetedEntityDestRect) {}

void IngameViewHandler::render() const {
    // Focus on player entity
    // auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_Texture* cachedRenderTarget = nullptr;
    SDL_SetRenderTarget(globals::renderer, mTexture);

    // Render dependencies
    std::invoke(kRenderMethod);

    switch (mView) {
        case View::kFullScreen:
            SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
            SDL_RenderCopy(globals::renderer, mTexture, nullptr, &mDestRect);
            break;

        case View::kTargetEntity:
            // Calculate rendered portion
            mViewport.x = mTargetedEntityDestRect.x + (mTargetedEntityDestRect.w - mViewport.w) / 2;
            mViewport.y = mTargetedEntityDestRect.y + (mTargetedEntityDestRect.h - mViewport.h) / 2;

            // "Fix" out-of-bound cases
            if (mViewport.x < 0) mViewport.x = 0;
            else if (mViewport.x + mViewport.w > mTextureSize.x) mViewport.x = mTextureSize.x - mViewport.w;
            if (mViewport.y < 0) mViewport.y = 0;
            else if (mViewport.y + mViewport.h > mTextureSize.y) mViewport.y = mTextureSize.y - mViewport.h;

            SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
            SDL_RenderCopy(globals::renderer, mTexture, &mViewport, nullptr);
            break;
    }
}

void IngameViewHandler::onWindowChange() {
    mTileCountWidth = static_cast<double>(globals::windowSize.x) / static_cast<double>(globals::windowSize.y) * mTileCountHeight;   // `mTileCountHeight` is immutable

    mViewport.w = mTileCountWidth * static_cast<double>(level::data.tileDestSize.x);
    mViewport.h = mTileCountHeight * static_cast<double>(level::data.tileDestSize.y);

    switch (mView) {
        case View::kFullScreen:
            // mTileDestSize.x = mTileDestSize.y = 1 << static_cast<int>(log2(std::min(globals::windowSize.x / level::data.tileDestCount.x, globals::windowSize.y / level::data.tileDestCount.y)));
            mTileDestSize.x = mTileDestSize.y = std::min(
                std::min(globals::windowSize.x / level::data.tileDestCount.x, globals::windowSize.y / level::data.tileDestCount.y) << 1,   // Sacrifice absolute "powers of 2" for decreased offset
                std::min(level::data.tileDestSize.x, level::data.tileDestSize.y) * 3 / 2
            );
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
    
    mTileCountHeight = level::data.viewportHeight;
    onWindowChange();

    // Switch view based on level size
    bool isLevelSizeSmallEnough = level::data.tileDestCount.x < mTileCountWidth && level::data.tileDestCount.y < mTileCountHeight;
    if ((mView == View::kFullScreen && !isLevelSizeSmallEnough) || (mView == View::kTargetEntity && isLevelSizeSmallEnough)) switchView();
}

/**
 * @see https://stackoverflow.com/questions/12761315/random-element-from-unordered-set-in-o1
*/
void IngameViewHandler::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case ~config::Key::kIngameCameraAngleToggle:
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