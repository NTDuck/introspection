#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericBoxComponent<T>::GenericBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<T>(center, preset) {}

template <typename T>
GenericBoxComponent<T>::~GenericBoxComponent() {
    if (mBoxTexture != nullptr) {
        SDL_DestroyTexture(mBoxTexture);
        mBoxTexture = nullptr;
    }
}

template <typename T>
void GenericBoxComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, mBoxTexture, nullptr, &mBoxDestRect);
}

template <typename T>
void GenericBoxComponent<T>::onWindowChange() {
    GenericComponent<T>::onWindowChange();
    loadBoxTexture(mBoxTexture, kPreset);
}

template <typename T>
void GenericBoxComponent<T>::shrinkRect(SDL_Rect& rect, const float ratio) {
    int delta = utils::ftoi(std::min(rect.w, rect.h) / 2 * ratio);
    rect.x += delta;
    rect.y += delta;
    rect.w -= delta * 2;
    rect.h -= delta * 2;
}

template <typename T>
void GenericBoxComponent<T>::loadBoxTexture(SDL_Texture*& texture, ComponentPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    
    mBoxDestRect.w = sDestSize * kDestRectRatio.x;
    mBoxDestRect.h = sDestSize * kDestRectRatio.y;
    mBoxDestRect.x = utils::ftoi(globals::windowSize.x * kCenter.x - mBoxDestRect.w / 2);
    mBoxDestRect.y = utils::ftoi(globals::windowSize.y * kCenter.y - mBoxDestRect.h / 2);

    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, 
    SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mBoxDestRect.w, mBoxDestRect.h);

    SDL_SetRenderTarget(globals::renderer, texture);
    SDL_Rect arbitraryRect = mBoxDestRect;
    arbitraryRect.x = arbitraryRect.y = 0;

    auto fillRect = [&](float multiplier, SDL_Color const& color) {
        shrinkRect(arbitraryRect, multiplier);
        utils::setRendererDrawColor(globals::renderer, color);
        SDL_RenderFillRect(globals::renderer, &arbitraryRect);
    };

    fillRect(0, preset.backgroundColor);
    fillRect(preset.lineOffset, preset.lineColor);
    fillRect(preset.lineWidth, preset.backgroundColor);

    SDL_SetRenderTarget(globals::renderer, nullptr);
}


template class GenericBoxComponent<FPSOverlay>;
template class GenericBoxComponent<IngameDialogueBox>;
template class GenericBoxComponent<MenuButton>;
template class GenericBoxComponent<LoadingProgressBar>;
template class GenericBoxComponent<GameOverButton>;