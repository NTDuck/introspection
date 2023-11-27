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

void Player::deinitialize() {
    tilesetData->deinitialize();
    delete instance;
    instance = nullptr;
}

/**
 * @brief Calculate player movement based on keyboard input.
 * @note Generates `nextDestCoords` and `nextDestRect`.
*/
void Player::handleKeyboardEvent(const SDL_Event& event) {
    // Only move if the player is not already moving
    if (nextDestCoords != nullptr) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: --currentVelocity.y; AbstractAnimatedDynamicEntity<Player>::initiateMove(); break;
        case SDLK_s: ++currentVelocity.y; AbstractAnimatedDynamicEntity<Player>::initiateMove(); break;
        case SDLK_a: --currentVelocity.x; AbstractAnimatedDynamicEntity<Player>::initiateMove(); break;
        case SDLK_d: ++currentVelocity.x; AbstractAnimatedDynamicEntity<Player>::initiateMove(); break;
        default: return;
    }
}

void Player::onLevelChange(const level::EntityLevelData& player) {
    auto data = dynamic_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(*data);
}


Player* Player::instance = nullptr;

template <>
const std::filesystem::path AbstractEntity<Player>::kTilesetPath = globals::config::kTilesetPathPlayer;