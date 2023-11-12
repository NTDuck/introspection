#include <filesystem>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


BaseTextureWrapper::BaseTextureWrapper() {}

BaseTextureWrapper::~BaseTextureWrapper() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    delete center;
}

/**
 * @brief Initialize the texture from a XML file.
 * @see <auxiliaries/utils.cpp> loadTilesetData (namespace method)
*/
void BaseTextureWrapper::init_(const std::filesystem::path xmlPath) {
    if (!std::filesystem::exists(xmlPath)) return;

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
    texture = IMG_LoadTexture(globals::renderer, (globals::config::ASSETS_PATH / path).string().c_str());
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
void BaseTextureWrapper::onLevelChange(const globals::leveldata::TextureData& texture) {
    destCoords = texture.destCoords;
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
 * @brief Retrieve a `SDL_Rect` representing the texture's position relative to the window.
*/
SDL_Rect BaseTextureWrapper::getDestRectFromCoords(const SDL_Point coords) {
    return {coords.x * globals::TILE_DEST_SIZE.x + globals::OFFSET.x, coords.y * globals::TILE_DEST_SIZE.y + globals::OFFSET.y, globals::TILE_DEST_SIZE.x, globals::TILE_DEST_SIZE.y};
};

/**
 * @brief Allow read-only access at public scope to protected attribute `destCoords`.
*/
SDL_Point BaseTextureWrapper::destCoords_getter() const {
    return destCoords;
}

bool BaseTextureWrapper::operator<(const BaseTextureWrapper& other) const {
    return (this -> destCoords.y == other.destCoords.y ? this -> destCoords.x < other.destCoords.x : this -> destCoords.y < other.destCoords.y);
}

bool BaseTextureWrapper::operator==(const BaseTextureWrapper& other) const {
    return (this -> destCoords.x == other.destCoords.x && this -> destCoords.y == other.destCoords.y);
}