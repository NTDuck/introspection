#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericTextComponent<T>::GenericTextComponent(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<T>(center, preset), content(content) {}

template <typename T>
GenericTextComponent<T>::~GenericTextComponent() {
    if (textTexture != nullptr) {
        SDL_DestroyTexture(textTexture);
        textTexture = nullptr;
    }
}

template <typename T>
void GenericTextComponent<T>::deinitialize() {
    Multiton<T>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

template <typename T>
void GenericTextComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, textTexture, nullptr, &textDestRect);
}

template <typename T>
void GenericTextComponent<T>::onWindowChange() {
    auto loadFont = [&]() {
        if (font != nullptr) {
            if (TTF_FontHeight(font) == destSize) return;
            TTF_CloseFont(font);
        }

        font = TTF_OpenFont(fontPath.generic_string().c_str(), destSize);   // Prevent `error: cannot convert 'const std::filesystem::__cxx11::path::value_type*' {aka 'const wchar_t*'} to 'const char*'`
    };

    GenericComponent<T>::onWindowChange();
    loadFont();
    loadTextTexture(textTexture, kPreset);
}

template <typename T>
void GenericTextComponent<T>::loadTextTexture(SDL_Texture*& texture, ComponentPreset const& preset) {
    if (texture != nullptr) SDL_DestroyTexture(texture);

    // Render text at high quality
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, content.c_str(), preset.textColor);
    if (surface == nullptr) return;

    texture = SDL_CreateTextureFromSurface(globals::renderer, surface);
    
    // Register `innerSurface` dimensions to `innerDestRect`
    textDestRect.w = surface->w;
    textDestRect.h = surface->h;
    textDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - textDestRect.w / 2);
    textDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - textDestRect.h / 2);

    SDL_FreeSurface(surface);
}

template <typename T>
void GenericTextComponent<T>::editContent(std::string const& nextContent) {
    content = nextContent;
    loadTextTexture(textTexture, kPreset);
}


template <typename T>
TTF_Font* GenericTextComponent<T>::font = nullptr;


template class GenericTextComponent<FrameRateOverlay>;
template class GenericTextComponent<MenuButton>;
template class GenericTextComponent<MenuTitle>;
template class GenericTextComponent<LoadingMessage>;