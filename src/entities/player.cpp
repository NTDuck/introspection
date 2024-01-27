#include <entities.hpp>

#include <SDL.h>

#include <filesystem>
#include <unordered_map>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @note Constructor initializer list is unusable.
*/
Player::Player(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<Player>(destCoords) {
    destRectModifier = config::entities::player::destRectModifier;
    kAttackRegisterRange = config::entities::player::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    primaryStats = config::entities::player::primaryStats;
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
    static SDL_Point prevDirection = { 1, 0 };

    auto arbHandleMovement = [&]() {
        static const std::unordered_map<SDL_Keycode, SDL_Point> mapping = {
            { config::key::PLAYER_MOVE_UP, {0, -1} },
            { config::key::PLAYER_MOVE_DOWN, {0, 1} },
            { config::key::PLAYER_MOVE_RIGHT, {-1, 0} },
            { config::key::PLAYER_MOVE_LEFT, {1, 0} },
        };

        auto it = mapping.find(event.key.keysym.sym);
        if (it == mapping.end()) return;

        if (event.type == SDL_KEYUP) {
            delete nextVelocity;
            nextVelocity = nullptr;
            return;
        }

        nextVelocity = new SDL_Point(it->second);
        prevDirection = it->second;
        initiateMove();
    };

    auto arbHandleProjectileAttack = [&]() {
        static const std::unordered_map<SDL_Keycode, ProjectileType> mapping = {
            { config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_SINGLE, ProjectileType::kOrthogonalSingle },
            { config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_DOUBLE, ProjectileType::kOrthogonalDouble },
            { config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_TRIPLE, ProjectileType::kOrthogonalTriple },
            { config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_QUADRUPLE, ProjectileType::kOrthogonalQuadruple },
            { config::key::PLAYER_SURGE_ATTACK_DIAGONAL_QUADRUPLE, ProjectileType::kDiagonalQuadruple },
        };

        auto it = mapping.find(event.key.keysym.sym);
        if (it == mapping.end()) return;

        if (event.type == SDL_KEYUP) return;

        PentacleProjectile::initiateAttack(it->second, destCoords, prevDirection);
    };

    if (currAnimationType == AnimationType::kDamaged || (nextAnimationType != nullptr && *nextAnimationType == AnimationType::kDamaged)) return;
    if (currAnimationType == AnimationType::kDeath || (nextAnimationType != nullptr && *nextAnimationType == AnimationType::kDeath)) return;

    switch (event.key.keysym.sym) {
        case config::key::PLAYER_MOVE_UP:
        case config::key::PLAYER_MOVE_DOWN:
        case config::key::PLAYER_MOVE_RIGHT:
        case config::key::PLAYER_MOVE_LEFT:
            arbHandleMovement(); break;

        case config::key::PLAYER_RUN_TOGGLE:
            onRunningToggled(event.type == SDL_KEYDOWN);
            break;

        case config::key::PLAYER_ATTACK:
            if (currAnimationType == AnimationType::kAttack || currAnimationType == AnimationType::kJump) break;
            resetAnimation(AnimationType::kAttack);
            break;

        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_SINGLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_DOUBLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_TRIPLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_QUADRUPLE:
        case config::key::PLAYER_SURGE_ATTACK_DIAGONAL_QUADRUPLE:
            if (currAnimationType == AnimationType::kAttack || currAnimationType == AnimationType::kJump) break;
            resetAnimation(AnimationType::kJump);
            arbHandleProjectileAttack(); break;

        default: break;
    }
}

void Player::onLevelChange(level::EntityLevelData const& player) {
    auto data = dynamic_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(*data);
}


template <>
int AbstractAnimatedDynamicEntity<Player>::kMoveDelay = config::entities::player::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Player>::kVelocity = config::entities::player::velocity;

template <>
const std::filesystem::path AbstractEntity<Player>::kTilesetPath = config::entities::player::path;