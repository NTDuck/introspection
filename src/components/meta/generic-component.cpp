#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


template <typename T>
GenericComponent<T>::GenericComponent(SDL_FPoint const& center, ComponentPreset const& preset) : kCenter(center), kPreset(preset) {}

template <typename T>
void GenericComponent<T>::onWindowChange() {
    destSize = std::min(globals::windowSize.x / kDestRectRatio.x, globals::windowSize.y / kDestRectRatio.y) >> 2;   // The closest power of 2
    destSize *= kDestSizeModifier;
}

template <typename T>
std::size_t std::hash<GenericComponent<T>>::operator()(GenericComponent<T> const*& instance) const {
    return instance == nullptr ? std::hash<std::nullptr_t>{}(instance) : std::hash<SDL_FPoint>(instance->kCenter);
}

template <typename T>
bool std::equal_to<GenericComponent<T>>::operator()(GenericComponent<T> const*& first, GenericComponent<T> const*& second) const {
    return (first == nullptr && second == nullptr) || (first && second && first->kCenter == second->kCenter);
}


template <typename T>
int GenericComponent<T>::destSize;


template class GenericComponent<FPSOverlay>;
template class GenericComponent<MenuButton>;
template class GenericComponent<MenuTitle>;
template class GenericComponent<LoadingMessage>;
template class GenericComponent<LoadingProgressBar>;