#include <components.hpp>

#include <string>
#include <SDL.h>

#include <mixer.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericButtonComponent<T>::GenericButtonComponent(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState) : GenericComponent<T>(center, onMouseOutPreset), GenericTextBoxComponent<T>(center, onMouseOutPreset, content), kOnMouseOverPreset(onMouseOverPreset), kDestState(destState) {}

/**
 * @see https://wiki.libsdl.org/SDL2/SDL_SystemCursor
*/
template <typename T>
void GenericButtonComponent<T>::initialize() {
    if (onMouseOutCursor == nullptr) onMouseOutCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);   // Typically the default cursor 
    if (onMouseOverCursor == nullptr) onMouseOverCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

template <typename T>
void GenericButtonComponent<T>::deinitialize() {
    if (onMouseOutCursor != nullptr) {
        SDL_FreeCursor(onMouseOutCursor);
        onMouseOutCursor = nullptr;
    }

    if (onMouseOverCursor != nullptr) {
        SDL_FreeCursor(onMouseOverCursor);
        onMouseOverCursor = nullptr;
    }

    GenericTextBoxComponent<T>::deinitialize();
}

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
    if (!isMouseOut) currAllMouseOut = false;
    if (!isMouseOut && event.type == SDL_MOUSEBUTTONDOWN) onClick();
}

/**
 * @brief Change cursor based on button "state" i.e. whether it is hovered.
 * @note Forward declaration of `SDL_Cursor` prevents accessing data members or using certain operators e.g. `delete`.
*/
template <typename T>
void GenericButtonComponent<T>::handleCursor() {
    if (currAllMouseOut != prevAllMouseOut) SDL_SetCursor(currAllMouseOut ? onMouseOutCursor : onMouseOverCursor);

    prevAllMouseOut = currAllMouseOut;
    currAllMouseOut = true;
}

template <typename T>
void GenericButtonComponent<T>::onClick() {
    if (kDestState == nullptr) return;
    Mixer::invoke(&Mixer::playSFX);
    globals::state = *kDestState;
}


template <typename T>
SDL_Cursor* GenericButtonComponent<T>::onMouseOutCursor = nullptr;

template <typename T>
SDL_Cursor* GenericButtonComponent<T>::onMouseOverCursor = nullptr;

template <typename T>
bool GenericButtonComponent<T>::prevAllMouseOut = true;

template <typename T>
bool GenericButtonComponent<T>::currAllMouseOut = true;


template class GenericButtonComponent<MenuButton>;
template class GenericButtonComponent<GameOverButton>;