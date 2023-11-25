#include <entities.hpp>

#include <filesystem>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>


/**
 * @note Constructor initializer list is unusable.
*/
Player::Player() {
    destRectModifier = {0, -23, 2, 2};
}

/**
 * @brief Creates an instance of this class, accessible as a static member.
 * @note Only one instance should exist at a time.
 * @todo Check for conflict with base class.
*/
Player* Player::instantiate() {
    if (instance == nullptr) instance = new Player;
    return instance;
}

/**
 * @see https://stackoverflow.com/questions/4146499/why-does-a-virtual-function-get-hidden
*/
void Player::initialize() {
    AbstractAnimatedDynamicEntity<Player>::initialize(globals::config::kTilesetPathPlayer);
}

void Player::deinitialize() {
    tilesetData->deinitialize();
    delete instance;
    instance = nullptr;
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
        case SDLK_w: --currentVelocity.y; break;
        case SDLK_s: ++currentVelocity.y; break;
        case SDLK_a: --currentVelocity.x; flip = SDL_FLIP_HORIZONTAL; break;
        case SDLK_d: ++currentVelocity.x; flip = SDL_FLIP_NONE; break;
        default: return;
    }

    nextDestCoords = new SDL_Point({destCoords.x + currentVelocity.x, destCoords.y + currentVelocity.y});
    nextDestRect = new SDL_Rect(AbstractAnimatedDynamicEntity::getDestRectFromCoords(*nextDestCoords));

    if (validateMove()) AbstractAnimatedDynamicEntity<Player>::onMoveStart(); else AbstractAnimatedDynamicEntity<Player>::onMoveEnd();
}

void Player::onLevelChange(const level::EntityLevelData& player) {
    auto data = dynamic_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(*data);
}

Player* Player::instance = nullptr;