#include <components.hpp>

#include <string>
#include <SDL.h>

#include <auxiliaries.hpp>


template <typename T>
GenericButtonComponent<T>::GenericButtonComponent(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState) : GenericComponent<T>(center, onMouseOutPreset), GenericTextBoxComponent<T>(center, onMouseOutPreset, content), kOnMouseOverPreset(onMouseOverPreset), kDestState(destState) {}

template <typename T>
void GenericButtonComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, (isMouseOut ? boxTexture : onMouseOverBoxTexture), nullptr, &boxDestRect);
    SDL_RenderCopy(globals::renderer, (isMouseOut ? textTexture : onMouseOverTextTexture), nullptr, &textDestRect);
}

template <typename T>
void GenericButtonComponent<T>::onWindowChange() {
    GenericTextBoxComponent<T>::onWindowChange();
    loadBoxTexture(onMouseOverBoxTexture, kOnMouseOverPreset);
    loadTextTexture(onMouseOverTextTexture, kOnMouseOverPreset);
}

template <typename T>
void GenericButtonComponent<T>::handleMouseEvent(SDL_Event const& event) {
    isMouseOut = !SDL_PointInRect(&globals::mouseState, &boxDestRect);
    if (!isMouseOut && event.type == SDL_MOUSEBUTTONDOWN) onClick();
}

template <typename T>
void GenericButtonComponent<T>::onClick() {
    if (kDestState == nullptr) return;
    globals::state = *kDestState;
}


template class GenericButtonComponent<MenuButton>;