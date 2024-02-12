#include <interface.hpp>

#include <type_traits>

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
    mTextureSize = globals::windowSize;
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextureSize.x, mTextureSize.y);
}


template class AbstractInterface<IngameMapHandler>;
template class AbstractInterface<IngameViewHandler>;
template class AbstractInterface<MenuInterface>;
template class AbstractInterface<LoadingInterface>;
template class AbstractInterface<GameOverInterface>;