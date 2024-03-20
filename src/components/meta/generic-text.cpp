#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericTextComponent<T>::GenericTextComponent(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<T>(center, preset), mContent(content) {}

template <typename T>
GenericTextComponent<T>::~GenericTextComponent() {
    if (mTextTexture != nullptr) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }
}

template <typename T>
void GenericTextComponent<T>::deinitialize() {
    Multiton<T>::deinitialize();
    if (sFont != nullptr) {
        TTF_CloseFont(sFont);
        sFont = nullptr;
    }
}

template <typename T>
void GenericTextComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, mTextTexture, nullptr, &mTextDestRect);
}

template <typename T>
void GenericTextComponent<T>::onWindowChange() {
    static constexpr auto loadFont = [&]() {
        if (sFont != nullptr) {
            if (TTF_FontHeight(sFont) == sDestSize) return;
            TTF_CloseFont(sFont);
        }

        sFont = TTF_OpenFont(sFontPath.generic_string().c_str(), sDestSize);   // Prevent `error: cannot convert 'const std::filesystem::__cxx11::path::value_type*' {aka 'const wchar_t*'} to 'const char*'`
    };

    GenericComponent<T>::onWindowChange();
    loadFont();
    loadTextTexture(mTextTexture, kPreset);
}

template <typename T>
void GenericTextComponent<T>::loadTextTexture(SDL_Texture*& texture, ComponentPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);

    // Render text at high quality
    SDL_Surface* surface = TTF_RenderUTF8_Blended(sFont, mContent.c_str(), preset.textColor);
    if (surface == nullptr) return;

    texture = SDL_CreateTextureFromSurface(globals::renderer, surface);
    
    // Register `innerSurface` dimensions to `innerDestRect`
    mTextDestRect.w = surface->w;
    mTextDestRect.h = surface->h;
    mTextDestRect.x = utils::ftoi(globals::windowSize.x * kCenter.x - mTextDestRect.w / 2);
    mTextDestRect.y = utils::ftoi(globals::windowSize.y * kCenter.y - mTextDestRect.h / 2);

    SDL_FreeSurface(surface);
}

template <typename T>
void GenericTextComponent<T>::editContent(std::string const& nextContent) {
    mContent = nextContent;
    loadTextTexture(mTextTexture, kPreset);
}


template <typename T>
TTF_Font* GenericTextComponent<T>::sFont = nullptr;


template class GenericTextComponent<FPSOverlay>;
template class GenericTextComponent<ExitText>;
template class GenericTextComponent<MenuButton>;
template class GenericTextComponent<MenuTitle>;
template class GenericTextComponent<LoadingMessage>;
template class GenericTextComponent<GameOverTitle>;
template class GenericTextComponent<GameOverButton>;