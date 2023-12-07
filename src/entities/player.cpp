#include <entities.hpp>

#include <filesystem>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>


/**
 * @note Constructor initializer list is unusable.
*/
Player::Player() {
    destRectModifier = globals::config::kDefaultPlayerDestRectModifier;
    kAttackRegisterRange = globals::config::kDefaultPlayerAttackRegisterRange;   // `kAttackInitiateRange` is unnecessary
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
    auto handleKeyboardMovementInput = [&]() {
        if (event.type == SDL_KEYUP) {
            nextVelocity = {0, 0};
            return;
        }
        switch (event.key.keysym.sym) {
            case SDLK_w: nextVelocity = {0, -1}; break;
            case SDLK_s: nextVelocity = {0, 1}; break;
            case SDLK_a: nextVelocity = {-1, 0}; break;
            case SDLK_d: nextVelocity = {1, 0}; break;
        }
        AbstractAnimatedDynamicEntity<Player>::initiateMove();
    };

    if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged || (nextAnimationData != nullptr && nextAnimationData->animationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged)) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: case SDLK_s: case SDLK_a: case SDLK_d:
            handleKeyboardMovementInput(); break;
        case SDLK_SPACE:
            if (currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kAttack) break;
            AbstractAnimatedDynamicEntity<Player>::onAttackInitiated(); break;
    }

}

void Player::onLevelChange(const level::EntityLevelData& player) {
    auto data = dynamic_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(*data);
}


Player* Player::instance = nullptr;


template <>
const int AbstractAnimatedDynamicEntity<Player>::kMoveDelay = globals::config::kDefaultPlayerMoveDelay;

template <>
const SDL_FPoint AbstractAnimatedDynamicEntity<Player>::kVelocity = globals::config::kDefaultPlayerVelocity;

template <>
const std::filesystem::path AbstractEntity<Player>::kTilesetPath = globals::config::kTilesetPathPlayer;