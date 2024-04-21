#include <components.hpp>

#include <string>
#include <functional>

#include <SDL.h>

#include <mixer.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericButtonComponent<T>::GenericButtonComponent(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState, std::function<void(void)> const& callback) : GenericComponent<T>(center, onMouseOutPreset), GenericTextBoxComponent<T>(center, onMouseOutPreset, content), kOnMouseOverPreset(onMouseOverPreset), kTargetGameState(destState), mCallback(callback) {}

/**
 * @see https://wiki.libsdl.org/SDL2/SDL_SystemCursor
*/
template <typename T>
void GenericButtonComponent<T>::initialize() {
    if (sOnMouseOutCursor == nullptr) sOnMouseOutCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);   // Typically the default cursor 
    if (sOnMouseOverCursor == nullptr) sOnMouseOverCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

template <typename T>
void GenericButtonComponent<T>::deinitialize() {
    if (sOnMouseOutCursor != nullptr) {
        SDL_FreeCursor(sOnMouseOutCursor);
        sOnMouseOutCursor = nullptr;
    }

    if (sOnMouseOverCursor != nullptr) {
        SDL_FreeCursor(sOnMouseOverCursor);
        sOnMouseOverCursor = nullptr;
    }

    GenericTextBoxComponent<T>::deinitialize();
}

template <typename T>
void GenericButtonComponent<T>::render() const {
    SDL_RenderCopy(globals::renderer, (mIsMouseOut ? mBoxTexture : mBoxTextureOnMouseOver), nullptr, &mBoxDestRect);
    SDL_RenderCopy(globals::renderer, (mIsMouseOut ? mTextTexture : mTextTextureOnMouseOver), nullptr, &mTextDestRect);
}

template <typename T>
void GenericButtonComponent<T>::onWindowChange() {
    GenericTextBoxComponent<T>::onWindowChange();
    loadBoxTexture(mBoxTextureOnMouseOver, kOnMouseOverPreset);
    loadTextTexture(mTextTextureOnMouseOver, kOnMouseOverPreset);
}

template <typename T>
void GenericButtonComponent<T>::handleMouseEvent(SDL_Event const& event) {
    mIsMouseOut = !SDL_PointInRect(&globals::mouseState, &mBoxDestRect);
    if (!mIsMouseOut) sCurrAllMouseOutState = false;
    if (!mIsMouseOut && event.type == SDL_MOUSEBUTTONDOWN) onClick();
}

/**
 * @brief Change cursor based on button "state" i.e. whether it is hovered.
 * @note Forward declaration of `SDL_Cursor` prevents accessing data members or using certain operators e.g. `delete`.
*/
template <typename T>
void GenericButtonComponent<T>::handleCursor() {
    if (sCurrAllMouseOutState != sPrevAllMouseOutState) SDL_SetCursor(sCurrAllMouseOutState ? sOnMouseOutCursor : sOnMouseOverCursor);

    sPrevAllMouseOutState = sCurrAllMouseOutState;
    sCurrAllMouseOutState = true;
}

template <typename T>
void GenericButtonComponent<T>::onClick() {
    std::invoke(mCallback);
    Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kButtonClick);

    if (kTargetGameState != nullptr) globals::state = *kTargetGameState;

    SDL_SetCursor(sOnMouseOutCursor);
}


template <typename T>
SDL_Cursor* GenericButtonComponent<T>::sOnMouseOutCursor = nullptr;

template <typename T>
SDL_Cursor* GenericButtonComponent<T>::sOnMouseOverCursor = nullptr;

template <typename T>
bool GenericButtonComponent<T>::sPrevAllMouseOutState = true;

template <typename T>
bool GenericButtonComponent<T>::sCurrAllMouseOutState = true;


template class GenericButtonComponent<MenuButton>;
template class GenericButtonComponent<GameOverButton>;