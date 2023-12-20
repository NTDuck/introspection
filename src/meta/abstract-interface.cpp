#include <meta.hpp>

#include <SDL.h>

#include <interface.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
AbstractInterface<T>::~AbstractInterface() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
}

template <class T>
void AbstractInterface<T>::render() {
    SDL_RenderCopy(globals::renderer, texture, nullptr, nullptr);
}

template <class T>
void AbstractInterface<T>::onWindowChange() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, globals::windowSize.x, globals::windowSize.y);
}

template class AbstractInterface<IngameInterface>;