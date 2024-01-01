#include <components.hpp>

#include <filesystem>
#include <string>

#include <SDL.h>

#include <auxiliaries.hpp>


template <typename T>
GenericTitle<T>::GenericTitle(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset) : GenericTextArea<T>(content, center, preset) {}

template <typename T>
void GenericTitle<T>::render() const {
    SDL_RenderCopy(globals::renderer, innerTexture, nullptr, &innerDestRect);
}


template class GenericTitle<MenuTitle>;