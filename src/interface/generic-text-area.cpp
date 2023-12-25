#include <interface.hpp>

#include <cstring>
#include <functional>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericTextArea<T>::GenericTextArea(std::string const& content, SDL_FPoint const& center) : content(content), kCenter(center) {}

template <typename T>
GenericTextArea<T>::~GenericTextArea() {
    SDL_DestroyTexture(outerTexture);
    SDL_DestroyTexture(innerTexture);
    outerTexture = innerTexture = nullptr;
}

template <typename T>
void GenericTextArea<T>::initialize() {
    font = TTF_OpenFont(globals::config::kOmoriFontSecondPath.c_str(), size);
    // if (font == nullptr) TTF_GetError();
}

template <typename T>
void GenericTextArea<T>::deinitialize() {
    Multiton<T>::deinitialize();
    TTF_CloseFont(font);
    font = nullptr;
}

/**
 * @brief Render the entire text area to the screen.
*/
template <typename T>
void GenericTextArea<T>::render() const {
    SDL_RenderCopy(globals::renderer, outerTexture, nullptr, &outerDestRect);
    SDL_RenderCopy(globals::renderer, innerTexture, nullptr, &innerDestRect);
}

/**
 * @brief Reset all `SDL_Texture*` and their corresponding `SDL_Rect`.
*/
template <typename T>
void GenericTextArea<T>::onWindowChange() {
    if (outerTexture != nullptr) SDL_DestroyTexture(outerTexture);
    if (innerTexture != nullptr) SDL_DestroyTexture(innerTexture);
    loadOuterTexture();
    loadInnerTexture();
}

template <typename T>
void GenericTextArea<T>::editContent(std::string const& nextContent) {
    content = nextContent;
    loadInnerTexture();
}

/**
 * @brief Populate the "outer" part of the text area.
*/
template <typename T>
void GenericTextArea<T>::loadOuterTexture() {
    constexpr static float kUnit = 2.0f / 32.0f;

    auto modifyRect = [](SDL_Rect& rect, float ratio) {
        int delta = utils::castFloatToInt(std::min(rect.w, rect.h) / 2 * ratio);
        rect.x += delta;
        rect.y += delta;
        rect.w -= delta * 2;
        rect.h -= delta * 2;
    };

    outerDestRect.w = size * 10;
    outerDestRect.h = size * 2;
    outerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - outerDestRect.w / 2);
    outerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - outerDestRect.h / 2);

    outerTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, 
    SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, outerDestRect.w, outerDestRect.h);

    SDL_SetRenderTarget(globals::renderer, outerTexture);
    SDL_Rect arbitraryRect = outerDestRect;
    arbitraryRect.x = arbitraryRect.y = 0;

    auto fillRect = [&](float multiplier, const SDL_Color& color) {
        modifyRect(arbitraryRect, kUnit * multiplier);
        utils::setRendererDrawColor(globals::renderer, color);
        SDL_RenderFillRect(globals::renderer, &arbitraryRect);
    };

    fillRect(0.0f, globals::config::kDefaultBlackColor);
    fillRect(0.5f, globals::config::kDefaultWhiteColor);
    fillRect(2.0f, globals::config::kDefaultBlackColor);

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

/**
 * @brief Populate the "text" part of the text area.
*/
template <typename T>
void GenericTextArea<T>::loadInnerTexture() {
    // Render text at high quality
    SDL_Surface* innerSurface = TTF_RenderUTF8_Blended(font, content.c_str(), globals::config::kDefaultWhiteColor);
    if (innerSurface == nullptr) return;

    innerTexture = SDL_CreateTextureFromSurface(globals::renderer, innerSurface);
    
    // Register `innerSurface` dimensions to `innerDestRect`
    innerDestRect.w = innerSurface->w;
    innerDestRect.h = innerSurface->h;
    innerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - innerDestRect.w / 2);
    innerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - innerDestRect.h / 2);

    SDL_FreeSurface(innerSurface);
}


/**
 * The global font in the scope of this project.
*/
template <typename T>
TTF_Font* GenericTextArea<T>::font = nullptr;


template class GenericTextArea<GenericButton>;