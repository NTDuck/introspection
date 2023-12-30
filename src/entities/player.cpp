#include <entities.hpp>

#include <filesystem>
#include <unordered_map>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @note Constructor initializer list is unusable.
*/
Player::Player(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<Player>(destCoords) {
    destRectModifier = config::player::destRectModifier;
    kAttackRegisterRange = config::player::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    primaryStats = config::player::primaryStats;
}

void Player::deinitialize() {
    tilesetData->deinitialize();
    tilesetData = nullptr;
    Singleton<Player>::deinitialize();
}

/**
 * @brief Calculate player movement based on keyboard input.
 * @note Generates `nextDestCoords` and `nextDestRect`.
*/
void Player::handleKeyboardEvent(SDL_Event const& event) {
    auto handleKeyboardMovementInput = [&]() {
        static const std::unordered_map<SDL_Keycode, SDL_Point> mapping = {
            { SDLK_w, {0, -1} },
            { SDLK_s, {0, 1} },
            { SDLK_a, {-1, 0} },
            { SDLK_d, {1, 0} },
        };

        auto it = mapping.find(event.key.keysym.sym);
        if (it == mapping.end()) return;

        if (event.type == SDL_KEYUP) {
            nextVelocity = {0, 0};
            return;
        }

        nextVelocity = it->second;
        AbstractAnimatedDynamicEntity<Player>::initiateMove();
    };

    if (currAnimationType == tile::EntitiesTilesetData::AnimationType::kDamaged || (nextAnimationData != nullptr && nextAnimationData->animationType == tile::EntitiesTilesetData::AnimationType::kDamaged)) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: case SDLK_s: case SDLK_a: case SDLK_d:
            handleKeyboardMovementInput(); break;
        case SDLK_SPACE:
            if (currAnimationType == tile::EntitiesTilesetData::AnimationType::kAttack) break;
            AbstractAnimatedDynamicEntity<Player>::onAttackInitiated(); break;
        case SDLK_LSHIFT: case SDLK_RSHIFT:
            AbstractAnimatedDynamicEntity<Player>::onRunningToggled(event.type == SDL_KEYDOWN);
            break;
    }
}

void Player::onLevelChange(level::EntityLevelData const& player) {
    auto data = dynamic_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(*data);
}


template <>
int AbstractAnimatedDynamicEntity<Player>::kMoveDelay = config::player::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Player>::kVelocity = config::player::velocity;

template <>
const std::filesystem::path AbstractEntity<Player>::kTilesetPath = config::player::path;