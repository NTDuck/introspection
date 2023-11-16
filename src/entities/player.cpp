#include <filesystem>

#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries/utils.hpp>


Player::Player() {}

Player::~Player() { AnimatedDynamicTextureWrapper::~AnimatedDynamicTextureWrapper(); }

/**
 * @see https://stackoverflow.com/questions/4146499/why-does-a-virtual-function-get-hidden
*/
void Player::init() {
    AnimatedDynamicTextureWrapper::init_(globals::config::PLAYER_TILESET_PATH);   // static_cast<NonStaticTextureWrapper*>(this) -> _init(config::PLAYER_TILESET_PATH);
}

/**
 * @brief Handle player movement based on keyboard input.
 * @note Generates `nextDestCoords` and `nextDestRect`.
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

    if (validateMove()) AnimatedDynamicTextureWrapper::onMoveStart(); else AnimatedDynamicTextureWrapper::onMoveEnd();
}

void Player::onLevelChange(const leveldata::TextureData& player) {
    auto data = dynamic_cast<const leveldata::PlayerData*>(&player);
    AnimatedDynamicTextureWrapper::onLevelChange(*data);
}

/**
 * @brief Render the current sprite.
 * @note Override base method to enlarge player without affecting `srcCoords`, which is used in many operations.
*/
void Player::render() {
    SDL_Rect* destRect_ = new SDL_Rect(destRect);
    destRect_ -> x -= globals::tileDestSize.x >> 1;
    destRect_ -> y -= globals::tileDestSize.y * 1.3;
    destRect_ -> w <<= 1;
    destRect_ -> h <<= 1;
    SDL_RenderCopyEx(globals::renderer, tilesetData.texture, &srcRect, destRect_, angle, center, flip);
    delete destRect_;
}