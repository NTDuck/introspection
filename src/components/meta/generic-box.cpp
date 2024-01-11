#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericBoxComponent<T>::GenericBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<T>(center, preset) {}

template <typename T>
GenericBoxComponent<T>::~GenericBoxComponent() {
    if (boxTexture != nullptr) {
        SDL_DestroyTexture(boxTexture);
        boxTexture = nullptr;
    }
}

template <typename T>
void GenericBoxComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, boxTexture, nullptr, &boxDestRect);
}

template <typename T>
void GenericBoxComponent<T>::onWindowChange() {
    GenericComponent<T>::onWindowChange();
    loadBoxTexture(boxTexture, kPreset);
}

template <typename T>
void GenericBoxComponent<T>::shrinkRect(SDL_Rect& rect, const float ratio) {
    int delta = utils::castFloatToInt(std::min(rect.w, rect.h) / 2 * ratio);
    rect.x += delta;
    rect.y += delta;
    rect.w -= delta * 2;
    rect.h -= delta * 2;
}

template <typename T>
void GenericBoxComponent<T>::loadBoxTexture(SDL_Texture*& texture, ComponentPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    
    boxDestRect.w = destSize * kDestRectRatio.x;
    boxDestRect.h = destSize * kDestRectRatio.y;
    boxDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - boxDestRect.w / 2);
    boxDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - boxDestRect.h / 2);

    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, 
    SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, boxDestRect.w, boxDestRect.h);

    SDL_SetRenderTarget(globals::renderer, texture);
    SDL_Rect arbitraryRect = boxDestRect;
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


template class GenericBoxComponent<FrameRateOverlay>;
template class GenericBoxComponent<MenuButton>;
template class GenericBoxComponent<LoadingProgressBar>;