#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include <meta.hpp>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


TextureWrapper::TextureWrapper(SDL_Point destCoords) : destCoords(destCoords) {
    velocity = {0, 0};
}

TextureWrapper::~TextureWrapper() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
}

/**
 * @brief Load the texture.
*/
void TextureWrapper::init(const std::string path) {
    texture = IMG_LoadTexture(globals::renderer, path.c_str());
}

/**
 * @brief Update attrbutes based on global variables.
*/
void TextureWrapper::blit() {
    // Requires `globals.TILE_DEST_SIZE` initialized in `Interface.loadLevel()`, exposed in `Interface.blit()`
    destRect = {
        destCoords.x * globals::TILE_DEST_SIZE.x, destCoords.y * globals::TILE_DEST_SIZE.y,
        32, 32,
    };
}

/**
 * @brief Render the current sprite.
 * @todo Needs further optimization.
 * @note Derived classes must properly set `destRect` and `srcRectsIndex` before calling this method, either during instantiation or via `init()` method.
 * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
*/
void TextureWrapper::render() {
    SDL_RenderCopyEx(globals::renderer, texture, &srcRect, &destRect, angle, center, flip);
}

/**
 * @brief Set color modulation on texture. Follows standard RGB color model.
*/
void TextureWrapper::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(texture, r, g, b);
}

/**
 * @brief Enable blending on texture. Required for alpha modulation.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
void TextureWrapper::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(texture, blendMode);
}

/**
 * @brief Set alpha modulation on texture.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
void TextureWrapper::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
}

/**
 * @brief A shorthand for setting both color modulation and alpha modulation on texture.
 * @param col represents a standard RGBA value.
*/
void TextureWrapper::setRGBA(SDL_Color color) {
    setRGB(color.r, color.g, color.b);
    setBlending();
    setAlpha(color.a);
}

/**
 * @brief Handle only the implementation of moving the texture from one `Tile` to the next.
 * @note Validation is not handled and should be implemented elsewhere, perferably by derived classes.
*/
void TextureWrapper::move() {
    if (!velocity.x && !velocity.y) return;

    destRect.x += velocity.x * VELOCITY.x;
    destRect.y += velocity.y * VELOCITY.y;

    // Terminate movement when reached new `Tile`
    if ((destRect.x % globals::TILE_DEST_SIZE.x) || (destRect.y % globals::TILE_DEST_SIZE.y)) return;

    // Update tileset-relative coordinates
    destCoords.x += velocity.x;
    destCoords.y += velocity.y;

    // Reset velocity
    velocity = {0, 0};
}