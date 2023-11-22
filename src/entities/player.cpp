#include <filesystem>

#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries/utils.hpp>


extern template class AnimatedDynamicTextureWrapper<Player>;
extern template class AnimatedTextureWrapper<Teleporter>;


Player::Player() {
    destRectModifier = {0, -23, 2, 2};
}

/**
 * @brief Creates an instance of this class, accessible as a static member.
 * @note Only one instance should exist at a time.
 * @todo Check for conflict with base class.
*/
void Player::instantiate() {
    if (instance == nullptr) return;
    instance = new Player;
}

/**
 * @see https://stackoverflow.com/questions/4146499/why-does-a-virtual-function-get-hidden
*/
void Player::initialize() {
    AnimatedDynamicTextureWrapper<Player>::initialize(globals::config::PLAYER_TILESET_PATH);
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

    if (validateMove()) AnimatedDynamicTextureWrapper<Player>::onMoveStart(); else AnimatedDynamicTextureWrapper<Player>::onMoveEnd();
}

void Player::onLevelChange(const leveldata::TextureData& player) {
    auto data = dynamic_cast<const leveldata::PlayerData*>(&player);
    AnimatedDynamicTextureWrapper<Player>::onLevelChange(*data);
}

Player* Player::instance = nullptr;