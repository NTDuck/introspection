#include <filesystem>
#include <sstream>

#include <meta.hpp>
#include <entities.hpp>
#include <auxiliaries/utils.hpp>


Player::Player() {}

Player::~Player() {
    NonStaticTextureWrapper::~NonStaticTextureWrapper();
}

/**
 * @see https://stackoverflow.com/questions/4146499/why-does-a-virtual-function-get-hidden
*/
void Player::init() {
    NonStaticTextureWrapper::initAbstract(config::PLAYER_TILESET_PATH);   // static_cast<NonStaticTextureWrapper*>(this) -> _init(config::PLAYER_TILESET_PATH);
}

/**
 * @brief Handle player movement based on keyboard input.
*/
void Player::handleKeyboardEvent(const SDL_Event& event) {
    // Only move if the player is not already moving
    if (nextDestCoords != nullptr) return;

    // if ((nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::TILE_DEST_COUNT.x || nextDestCoords -> y >= globals::TILE_DEST_COUNT.y)) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: --velocity.y; break;
        case SDLK_s: ++velocity.y; break;
        case SDLK_a: --velocity.x; flip = SDL_FLIP_HORIZONTAL; break;
        case SDLK_d: ++velocity.x; flip = SDL_FLIP_NONE; break;
        default: return;
    }

    nextDestCoords = new SDL_Point({destCoords.x + velocity.x, destCoords.y + velocity.y});
    nextDestRect = new SDL_Rect(BaseTextureWrapper::getDestRectFromCoords(*nextDestCoords));

    // Check if the next `Tile` is valid
    if (validateMove()) NonStaticTextureWrapper::onMoveStart(); else NonStaticTextureWrapper::onMoveEnd();
}

/**
 * @brief Render the current sprite.
 * @note Override base method to enlarge player without affecting `srcCoords`, which is used in many operations.
*/
void Player::render() {
    SDL_Rect* _destRect = new SDL_Rect(destRect);
    _destRect -> x -= globals::TILE_DEST_SIZE.x * 0.5;
    _destRect -> y -= globals::TILE_DEST_SIZE.y * 1.3;
    _destRect -> w *= 2;
    _destRect -> h *= 2;
    SDL_RenderCopyEx(globals::renderer, texture, &srcRect, _destRect, angle, center, flip);
}