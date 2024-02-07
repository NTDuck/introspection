#include <interface.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractInterface<T>::~AbstractInterface() {
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

template <typename T>
void AbstractInterface<T>::render() const {
    SDL_RenderCopy(globals::renderer, mTexture, nullptr, nullptr);
}

template <typename T>
void AbstractInterface<T>::onWindowChange() {
    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, globals::windowSize.x, globals::windowSize.y);
}


template class AbstractInterface<IngameMapHandler>;
template class AbstractInterface<IngameViewHandler>;
template class AbstractInterface<MenuInterface>;
template class AbstractInterface<LoadingInterface>;
template class AbstractInterface<GameOverInterface>;