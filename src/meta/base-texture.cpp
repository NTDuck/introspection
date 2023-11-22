#include <filesystem>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
BaseTextureWrapper<T>::BaseTextureWrapper() {
    srcRect.w = tilesetData->srcSize.x * tilesetData->animationSize.x;
    srcRect.h = tilesetData->srcSize.y * tilesetData->animationSize.y;
    destRectModifier = {0, 0, 1, 1};
}

/**
 * @brief Creates an instance of derived class `T`. Also registers to `instanceMapping`.
*/
template <class T>
T* BaseTextureWrapper<T>::instantiate(SDL_Point destCoords) {
    auto instance = new T;
    instance->destCoords = destCoords;   // Intellisense not working properly here, might prone to error
    instanceMapping.emplace(std::make_pair(destCoords, instance));
    return instance;
}

/**
 * @brief Deletes an instance of derived class `T`. Also removes from `instanceMapping`.
*/
template <class T>
BaseTextureWrapper<T>::~BaseTextureWrapper() {
    instanceMapping.erase(destCoords);   // by key
}

/**
 * @brief Initializes derived class `T`.
 * @see <auxiliaries/utils.cpp> loadTilesetData (namespace method). Share many similarities but could not merge due to lack of motivation.
*/
template <class T>
void BaseTextureWrapper<T>::initialize(const std::filesystem::path xmlPath) {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(xmlPath.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    tilesetData->init(document, globals::renderer);
}

template <class T>
void BaseTextureWrapper<T>::terminate() {
    instanceMapping.clear();
    tilesetData->dealloc();
}

/**
 * @brief Render the current sprite.
 * @note Derived classes must properly set `destRect` and `srcRectsIndex` before calling this method, either during instantiation or via `init()` method.
 * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
*/
template <class T>
void BaseTextureWrapper<T>::render() const {
    SDL_RenderCopyEx(globals::renderer, tilesetData->texture, &srcRect, &destRect, angle, &center, flip);
}

/**
 * @brief Update attributes that depend on window.
*/
template <class T>
void BaseTextureWrapper<T>::onWindowChange() {
    destRect = getDestRectFromCoords(destCoords);
}

/**
 * @brief Update attributes that change when level changes.
 * @note Does nothing by default, implementation will be handled by derived classes.
 * @note Behold, polyphormism!
*/
template <class T>
void BaseTextureWrapper<T>::onLevelChange(const leveldata::TextureData& texture) {
    destCoords = texture.destCoords;
}

template <class T>
void BaseTextureWrapper<T>::renderAll() {
    for (auto& pair : instanceMapping) pair.second->render();
}

template <class T>
void BaseTextureWrapper<T>::onWindowChangeAll() {
    for (auto& pair : instanceMapping) pair.second->onWindowChange();
}

/**
 * @brief Sets color modulation on the texture of derived class `T`.
*/
template <class T>
void BaseTextureWrapper<T>::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(tilesetData->texture, r, g, b);
}

/**
 * @brief Enables blending on the texture of derived class `T`.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @note Required for alpha modulation.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
template <class T>
void BaseTextureWrapper<T>::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(tilesetData->texture, blendMode);
}

/**
 * @brief Sets alpha modulation on the texture of derived class `T`.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
template <class T>
void BaseTextureWrapper<T>::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(tilesetData->texture, alpha);
}

/**
 * @brief Sets both color modulation and alpha modulation on the texture of derived class `T`.
 * @param col represents a standard RGBA value.
*/
template <class T>
void BaseTextureWrapper<T>::setRGBA(SDL_Color color) {
    setRGB(color.r, color.g, color.b);
    setBlending();
    setAlpha(color.a);
}

/**
 * @return A `SDL_Rect` representing the position of the instance of derived class `T`, relative to the window.
 * @note The following calculations bear fruition from several commits, and should be understood as-is. Additional explanation will not be provided elsewhere.
*/
template <class T>
SDL_Rect BaseTextureWrapper<T>::getDestRectFromCoords(const SDL_Point coords) {
    return {
        coords.x * globals::tileDestSize.x + globals::windowOffset.x + destRectModifier.x - ((globals::tileDestSize.x * tilesetData->animationSize.x * (destRectModifier.w - 1)) >> 1),
        coords.y * globals::tileDestSize.y + globals::windowOffset.y + destRectModifier.y - ((globals::tileDestSize.y * tilesetData->animationSize.y * (destRectModifier.h - 1)) >> 1),
        globals::tileDestSize.x * tilesetData->animationSize.x * destRectModifier.w,
        globals::tileDestSize.y * tilesetData->animationSize.y * destRectModifier.h,
    };
};


/**
 * @brief Maps the `destCoords` of each active instance of derived class `T` to its corresponding pointer. Useful for determining interactions between different instances of different derived classes e.g. entity collision.
*/
template <class T>
std::unordered_map<SDL_Point, T*, utils::hashers::SDL_Point_Hasher, utils::operators::SDL_Point_Equality_Operator> BaseTextureWrapper<T>::instanceMapping;

template <class T>
tiledata::AnimatedEntitiesTilesetData* BaseTextureWrapper<T>::tilesetData = nullptr;