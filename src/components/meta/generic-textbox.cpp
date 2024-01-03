#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericTextBoxComponent<T>::GenericTextBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<T>(center, preset), GenericTextComponent<T>(center, preset, content), GenericBoxComponent<T>(center, preset) {}

template <typename T>
void GenericTextBoxComponent<T>::deinitialize() {
    GenericTextComponent<T>::deinitialize();
    GenericBoxComponent<T>::deinitialize();
}

template <typename T>
void GenericTextBoxComponent<T>::render() const {
    GenericTextComponent<T>::render();
    GenericBoxComponent<T>::render();
}

template <typename T>
void GenericTextBoxComponent<T>::onWindowChange() {
    GenericTextComponent<T>::onWindowChange();
    GenericBoxComponent<T>::onWindowChange();
}


template class GenericTextBoxComponent<MenuButton>;