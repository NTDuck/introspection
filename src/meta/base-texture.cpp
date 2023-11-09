#include <filesystem>
#include <SDL.h>
#include <SDL_image.h>

#include <meta.hpp>
#include <pugixml/pugixml.hpp>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


BaseTextureWrapper::BaseTextureWrapper() {}

BaseTextureWrapper::~BaseTextureWrapper() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    delete nextDestCoords;
    delete nextDestRect;
    delete center;
}

/**
 * @brief Initialize the texture from a XML file.
 * @see <auxiliaries/utils.cpp> loadTilesetData (namespace method)
*/
void BaseTextureWrapper::initAbstract(const std::filesystem::path xmlPath) {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(xmlPath.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    // Parse nodes
    pugi::xml_node tilesetNode = document.child("tileset");
    pugi::xml_node propertiesNode = tilesetNode.child("properties");
    pugi::xml_node imageNode = tilesetNode.child("image");

    // `srcRect` dimensions (constant)
    srcRect.w = tilesetNode.attribute("tilewidth").as_int();
    srcRect.h = tilesetNode.attribute("tileheight").as_int();

    // Custom properties, likely `srcRect`
    for (pugi::xml_node propertyNode = propertiesNode.child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property")) properties[propertyNode.attribute("name").as_string()] = propertyNode.attribute("value").as_string();

    // Load texture
    std::filesystem::path path (imageNode.attribute("source").value());
    utils::cleanRelativePath(path);
    texture = IMG_LoadTexture(globals::renderer, (config::ASSETS_PATH / path).string().c_str());

    VELOCITY = config::VELOCITY_PLAYER;
    velocity = {0, 0};

    onMoveEnd();
}

/**
 * @brief Update certain attrbutes when global variables change e.g. window resize, new level.
*/
void BaseTextureWrapper::blit() {
    // Requires `globals.TILE_DEST_SIZE` initialized in `Interface.loadLevel()`, exposed in `Interface.blit()`
    destCoords = globals::currentLevel.playerDestCoords;
    destRect = getDestRectFromCoords(destCoords);
    onMoveEnd();
}

/**
 * @brief Render the current sprite.
 * @note Derived classes must properly set `destRect` and `srcRectsIndex` before calling this method, either during instantiation or via `init()` method.
 * @see https://wiki.libsdl.org/SDL2/SDL_RendererFlip
*/
void BaseTextureWrapper::render() {
    SDL_RenderCopyEx(globals::renderer, texture, &srcRect, &destRect, angle, center, flip);
}

/**
 * @brief Set color modulation on `texture`. Follows standard RGB color model.
*/
void BaseTextureWrapper::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(texture, r, g, b);
}

/**
 * @brief Enable blending on `texture`. Required for alpha modulation.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
void BaseTextureWrapper::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(texture, blendMode);
}

/**
 * @brief Set alpha modulation on `texture`.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
void BaseTextureWrapper::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
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
 * @brief Handle only the implementation of moving the texture from one `Tile` to the next.
 * @note Requires `nextDestCoords` and `nextDestRect` configured by derived classes. Validation is not handled and should be implemented by derived classes.
*/
void BaseTextureWrapper::move() {
    if (nextDestCoords == nullptr) return;   // DO NOT remove this - without this the program magically terminates itself.

    destRect.x += velocity.x * VELOCITY.x;
    destRect.y += velocity.y * VELOCITY.y;

    // Continue movement until new `Tile` has been reached.
    if ((nextDestRect -> x - destRect.x) * velocity.x > 0 || (nextDestRect -> y - destRect.y) * velocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway

    // Terminate movement when reached new `Tile`
    destCoords = *nextDestCoords;
    destRect = *nextDestRect;
    onMoveEnd();
}

/**
 * @brief Check whether moving the texture from one tile to another is valid.
 * @note Only check whether next tile exceeds map limit. Further validation should be separately implemented by derived classes.
*/
bool BaseTextureWrapper::validateMove() {
    if (nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::TILE_DEST_COUNT.x || nextDestCoords -> y >= globals::TILE_DEST_COUNT.y) return false;
    return true;
}

/**
 * @brief Retrieve a `SDL_Rect` representing the texture's position relative to the window.
*/
SDL_Rect BaseTextureWrapper::getDestRectFromCoords (const SDL_Point coords) {
    return SDL_Rect {coords.x * globals::TILE_DEST_SIZE.x + globals::OFFSET.x, coords.y * globals::TILE_DEST_SIZE.y + globals::OFFSET.y, globals::TILE_DEST_SIZE.x, globals::TILE_DEST_SIZE.y};
};

/**
 * @brief Clean up resources after entity finished moving.
*/
void BaseTextureWrapper::onMoveEnd() {
    nextDestCoords = nullptr;
    nextDestRect = nullptr;

    // Reset velocity
    velocity = {0, 0};
}