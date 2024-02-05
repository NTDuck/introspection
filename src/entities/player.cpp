#include <entities.hpp>

#include <filesystem>
#include <unordered_map>

#include <SDL.h>

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

void Player::onLevelChange(level::EntityLevelData const& player) {
    auto data = *reinterpret_cast<const level::PlayerLevelData*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(data);
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

void Player::handleCustomEventPOST() const {
    handleCustomEventPOST_kReq_AttackRegister_Player_GHE();
    handleCustomEventPOST_kReq_Death_Player();
}

void Player::handleCustomEventGET(SDL_Event const& event) {
    switch (static_cast<event::Code>(event.user.code)) {
        case event::Code::kReq_AttackRegister_GHE_Player:
            handleCustomEventGET_kReq_AttackRegister_GHE_Player(event);
            break;

        case event::Code::kReq_AttackInitiate_GHE_Player:
            handleCustomEventGET_kResp_AttackInitiate_GHE_Player(event);
            break;

        case event::Code::kReq_MoveInitiate_GHE_Player:
            handleCustomEventGET_kResp_MoveInitiate_GHE_Player(event);
            break;

        case event::Code::kReq_Teleport_GTE_Player:
            handleCustomEventGET_kResp_Teleport_GTE_Player(event);
            break;

        default: break;
    }
}

void Player::handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const {
    if (currAnimationType != AnimationType::kAttack || !isAnimationAtFinalSprite()) return;

    auto event = formatCustomEvent();
    populateCustomEvent(event, event::Code::kReq_AttackRegister_Player_GHE, event::data::kReq_AttackRegister_Player_GHE({ destCoords, kAttackRegisterRange, secondaryStats }));
    enqueueCustomEvent(event);
}

void Player::handleCustomEventPOST_kReq_Death_Player() const {
    if (currAnimationType != AnimationType::kDeath) return;

    static int counter = config::entities::player::waitingFramesAfterDeath;
    if (counter > 0) --counter;

    auto event = formatCustomEvent();
    populateCustomEvent(event, counter == 0 ? event::Code::kReq_DeathFinalized_Player : event::Code::kReq_DeathPending_Player, counter);   // Placeholder
    enqueueCustomEvent(event);
}

void Player::handleCustomEventGET_kReq_AttackRegister_GHE_Player(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_AttackRegister_GHE_Player*>(event.user.data1);

    if (currAnimationType == AnimationType::kDamaged) return;
    auto distance = utils::calculateDistance(destCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(data.stats, secondaryStats);
    secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedMagicDamage(data.stats, secondaryStats);

    if (nextAnimationType == nullptr) {
        nextAnimationType = new AnimationType(secondaryStats.HP > 0 ? AnimationType::kDamaged : AnimationType::kDeath);
        isAnimationOnProgress = false;
    }
}

void Player::handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_AttackInitiate_GHE_Player*>(event.user.data1);

    if (currAnimationType == AnimationType::kDamaged || currAnimationType == AnimationType::kDeath) return;
    auto distance = utils::calculateDistance(destCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    auto event_ = formatCustomEvent(*reinterpret_cast<int*>(event.user.data2));
    populateCustomEvent(event_, event::Code::kResp_AttackInitiate_GHE_Player, data);
    enqueueCustomEvent(event_);
}

void Player::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_MoveInitiate_GHE_Player*>(event.user.data1);

    if (currAnimationType == AnimationType::kDeath) return;
    auto distance = utils::calculateDistance(destCoords, data.destCoords);

    auto event_ = formatCustomEvent(*reinterpret_cast<int*>(event.user.data2));
    data.destCoords = destCoords;
    populateCustomEvent(event_, distance > data.range.x || distance > data.range.y ? event::Code::kResp_MoveTerminate_GHE_Player : event::Code::kResp_MoveInitiate_GHE_Player, data);
    enqueueCustomEvent(event_);
}

void Player::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_Teleport_GTE_Player*>(event.user.data1);

    if (destCoords != data.destCoords) return;

    auto event_ = formatCustomEvent(*reinterpret_cast<int*>(event.user.data2));
    populateCustomEvent(event_, event::Code::kResp_Teleport_GTE_Player, data);
    enqueueCustomEvent(event_);
}


template <>
int AbstractAnimatedDynamicEntity<Player>::kMoveDelay = config::entities::player::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Player>::kVelocity = config::entities::player::velocity;

template <>
const std::filesystem::path AbstractEntity<Player>::kTilesetPath = config::entities::player::path;