#include <components.hpp>

#include <string>
#include <SDL.h>

#include <auxiliaries.hpp>


template <typename T>
GenericButton<T>::GenericButton(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver) : GenericTextArea<T>(content, center, preset), kDestState(destState), kPresetOnMouseOver(presetOnMouseOver) {}

template <typename T>
void GenericButton<T>::render() const {
    SDL_RenderCopy(globals::renderer, (isMouseOut ? outerTexture : outerTextureOnMouseOver), nullptr, &outerDestRect);
    SDL_RenderCopy(globals::renderer, (isMouseOut ? innerTexture : innerTextureOnMouseOver), nullptr, &innerDestRect);
}

template <typename T>
void GenericButton<T>::onWindowChange() {
    GenericTextArea<T>::onWindowChange();
    loadOuterTexture(outerTextureOnMouseOver, kPresetOnMouseOver);
    loadInnerTexture(innerTextureOnMouseOver, kPresetOnMouseOver);
}

template <typename T>
void GenericButton<T>::handleMouseEvent(SDL_Event const& event) {
    isMouseOut = !SDL_PointInRect(&globals::mouseState, &outerDestRect);
    if (!isMouseOut && event.type == SDL_MOUSEBUTTONDOWN) onClick();
}

template <typename T>
void GenericButton<T>::onClick() {
    if (kDestState == nullptr) return;
    globals::state = *kDestState;
}


template class GenericButton<MenuButton>;