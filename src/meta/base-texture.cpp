#include <filesystem>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


BaseTextureWrapper::BaseTextureWrapper() {}

BaseTextureWrapper::~BaseTextureWrapper() {
    tilesetData.dealloc();
    delete center;
}

/**
 * @brief Initialize the texture from a XML file.
 * @see <auxiliaries/utils.cpp> loadTilesetData (namespace method). Share many similarities but could not merge due to lack of motivation.
*/
void BaseTextureWrapper::init_(const std::filesystem::path xmlPath) {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(xmlPath.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    tilesetData.init(document, globals::renderer);

    srcRect.w = tilesetData.srcSize.x * tilesetData.animationSize.x;
    srcRect.h = tilesetData.srcSize.y * tilesetData.animationSize.y;
}

/**
 * @brief Update attributes that depend on window.
*/
void BaseTextureWrapper::onWindowChange() {
    destRect = getDestRectFromCoords(destCoords);
}

/**
 * @brief Update attributes that change when level changes.
 * @note Does nothing by default, implementation will be handled by derived classes.
 * @note Behold, polyphormism!
*/
void BaseTextureWrapper::onLevelChange(const leveldata::TextureData& texture) {
    destCoords = texture.destCoords;
}

/**
 * @brief Render the current sprite.
 * @note Derived classes must properly set `destRect` and `srcRectsIndex` before calling this method, either during instantiation or via `init()` method.
 * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
*/
void BaseTextureWrapper::render() {
    SDL_RenderCopyEx(globals::renderer, tilesetData.texture, &srcRect, &destRect, angle, center, flip);
}

/**
 * @brief Set color modulation on `texture`. Follows standard RGB color model.
*/
void BaseTextureWrapper::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(tilesetData.texture, r, g, b);
}

/**
 * @brief Enable blending on `texture`. Required for alpha modulation.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
void BaseTextureWrapper::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(tilesetData.texture, blendMode);
}

/**
 * @brief Set alpha modulation on `texture`.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
void BaseTextureWrapper::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(tilesetData.texture, alpha);
}

/**
 * @brief A shorthand for setting both color modulation and alpha modulation on `texture`.
 * @param col represents a standard RGBA value.
*/
void BaseTextureWrapper::setRGBA(SDL_Color color) {
    setRGB(color.r, color.g, color.b);
    setBlending();
    setAlpha(color.a);
}

/**
 * @brief Retrieve a `SDL_Rect` representing the texture's position relative to the window.
 * @note The following calculations bear fruition from several commits, and should be understood as-is. Additional explanation will not be provided elsewhere.
*/
SDL_Rect BaseTextureWrapper::getDestRectFromCoords(const SDL_Point coords) {
    return {
        coords.x * globals::tileDestSize.x + globals::windowOffset.x + destRectModifier.x - ((globals::tileDestSize.x * tilesetData.animationSize.x * (destRectModifier.w - 1)) >> 1),
        coords.y * globals::tileDestSize.y + globals::windowOffset.y + destRectModifier.y - ((globals::tileDestSize.y * tilesetData.animationSize.y * (destRectModifier.h - 1)) >> 1),
        globals::tileDestSize.x * tilesetData.animationSize.x * destRectModifier.w,
        globals::tileDestSize.y * tilesetData.animationSize.y * destRectModifier.h,
    };
};

/**
 * @brief Allow read-only access at public scope to protected attribute `destCoords`.
*/
SDL_Point BaseTextureWrapper::destCoords_getter() const { return destCoords; }