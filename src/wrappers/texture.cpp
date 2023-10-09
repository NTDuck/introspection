#include <iostream>
#include <set>

#include <SDL.h>
#include <SDL_image.h>

#include <wrappers/texture.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <utils/sdl2.h>


TextureWrapper::TextureWrapper() {}

TextureWrapper::~TextureWrapper() {
    dealloc();
}

/**
 * @brief Prepare the spritesheet(s) for rendering.
*/
void TextureWrapper::loadSpriteSheet(std::set<TextureWrapperInitData> sheetData) {
    for (const auto& data: sheetData) {
        TextureWrapperMappingData entry;
        entry.texture = data.texture;
        entry.rects = utils::createRects(data);
        
        spriteMapping[data.state] = entry;
    }       
}

/**
 * @brief Deallocate the texture.
 * @note Called in deconstructor and on spritesheet load.
*/
void TextureWrapper::dealloc() {
    for (const auto& pair : spriteMapping) SDL_DestroyTexture(pair.second.texture);
    spriteMapping.clear();
}

/**
 * @brief Controls logic behind animation speed (i.e. how fast sprites update).
 * @warning This method is called every loop; calculating vector size might affect performance. Consider binding the vector size to a non-constant variable, if necessary.
*/
void TextureWrapper::nextSprite() {
    if (srcRectIndex >= static_cast<unsigned char>(spriteMapping.find(state) -> second.rects.size()) * textureUpdateRate) {
        srcRectIndex = 0;
        if (!utils::isPermanent(state)) {
            restoreState();
        }
    } srcRectIndex++;
}

/**
 * @brief Store the current "permanent" `SpriteState` when a "temporary" `SpriteState` occurs.
*/
void TextureWrapper::storeState(SpriteState tempState) {
    statePrev = state;
    state = tempState;
}

/**
 * @brief Restore a "permanent" `SpriteState` after a "temporary" `SpriteState` has finished.
*/
void TextureWrapper::restoreState() {
    state = statePrev;
    statePrev = SpriteState::IDLE;
}

/**
 * @brief Change the current state only if having higher "priority".
 * @see <auxiliaries/defs.h> SpriteState (enum)
*/
void TextureWrapper::setPriorityState(SpriteState newState) {
    if (state < newState) state = newState;
}

/**
 * @brief Render the current sprite.
 * @todo Needs further optimization.
 * @note Derived classes must properly set `destRect` and `srcRectsIndex` before calling this method, either during instantiation or via `init()` method.
 * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
*/
void TextureWrapper::render(SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, spriteMapping.find(state) -> second.texture, &spriteMapping.find(state) -> second.rects[srcRectIndex / textureUpdateRate], &destRect, angle, center, flip);
    nextSprite();
}

/**
 * @brief Set color modulation on texture. Follows standard RGB color model.
*/
void TextureWrapper::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(spriteMapping.find(state) -> second.texture, r, g, b);
}

/**
 * @brief Enable blending on texture. Required for alpha modulation.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
void TextureWrapper::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(spriteMapping.find(state) -> second.texture, blendMode);
}

/**
 * @brief Set alpha modulation on texture.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
void TextureWrapper::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(spriteMapping.find(state) -> second.texture, alpha);
}

/**
 * @brief A shorthand for setting both color modulation and alpha modulation on texture.
 * @param col represents a standard RGBA value.
*/
void TextureWrapper::setRGBA(SDL_Color col) {
    setRGB(col.r, col.g, col.b);
    setBlending();
    setAlpha(col.a);
}