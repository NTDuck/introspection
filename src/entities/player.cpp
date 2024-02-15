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
    mDestRectModifier = config::entities::player::destRectModifier;
    mAttackRegisterRange = config::entities::player::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    mPrimaryStats = config::entities::player::primaryStats;
}

void Player::deinitialize() {
    sTilesetData.deinitialize();
    Singleton<Player>::deinitialize();
}

void Player::reinitialize(bool increment) {
    static unsigned short int index = 0;
    static const unsigned short int size = static_cast<unsigned short int>(sTilesetPaths.size());

    if (increment) {
        if (index == size - 1) index = 0; else ++index;
    } else {
        if (!index) index = size - 1; else --index;
    }

    AbstractEntity<Player>::reinitialize(sTilesetPaths[index]);
}

void Player::onLevelChange(level::Data_Generic const& player) {
    auto data = *reinterpret_cast<const level::Data_Generic*>(&player);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(data);
}

/**
 * @brief Calculate player movement based on keyboard input.
 * @note Generates `nextDestCoords` and `nextDestRect`.
*/
void Player::handleKeyboardEvent(SDL_Event const& event) {
    if (mCurrAnimationType == AnimationType::kDamaged || (pNextAnimationType != nullptr && *pNextAnimationType == AnimationType::kDamaged)) return;
    if (mCurrAnimationType == AnimationType::kDeath || (pNextAnimationType != nullptr && *pNextAnimationType == AnimationType::kDeath)) return;

    switch (event.key.keysym.sym) {
        case config::key::PLAYER_MOVE_UP:
        case config::key::PLAYER_MOVE_DOWN:
        case config::key::PLAYER_MOVE_RIGHT:
        case config::key::PLAYER_MOVE_LEFT:
            handleKeyboardEvent_Movement(event);
            break;

        case config::key::PLAYER_RUN_TOGGLE:
            onRunningToggled(event.type == SDL_KEYDOWN);
            break;

        case config::key::PLAYER_ATTACK:
            if (mCurrAnimationType == AnimationType::kAttackMeele || mCurrAnimationType == AnimationType::kAttackRanged) break;
            resetAnimation(AnimationType::kAttackMeele);
            break;

        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_SINGLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_DOUBLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_TRIPLE:
        case config::key::PLAYER_SURGE_ATTACK_ORTHOGONAL_QUADRUPLE:
        case config::key::PLAYER_SURGE_ATTACK_DIAGONAL_QUADRUPLE:
            if (mCurrAnimationType == AnimationType::kAttackMeele || mCurrAnimationType == AnimationType::kAttackRanged) break;
            resetAnimation(AnimationType::kAttackRanged);
            handleKeyboardEvent_ProjectileAttack(event);
            break;

        default: break;
    }
}

void Player::handleMouseEvent(SDL_Event const& event) {
    switch (event.type) {
        case SDL_MOUSEWHEEL:
            reinitialize(event.wheel.y > 0);
            break;

        default: break;
    }
}

void Player::handleCustomEventPOST() const {
    handleCustomEventPOST_kReq_AttackRegister_Player_GHE();
    handleCustomEventPOST_kReq_Death_Player();
}

void Player::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
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

void Player::handleKeyboardEvent_Movement(SDL_Event const& event) {
    static const std::unordered_map<SDL_Keycode, SDL_Point> mapping = {
        { config::key::PLAYER_MOVE_UP, {0, -1} },
        { config::key::PLAYER_MOVE_DOWN, {0, 1} },
        { config::key::PLAYER_MOVE_RIGHT, {-1, 0} },
        { config::key::PLAYER_MOVE_LEFT, {1, 0} },
    };

    auto it = mapping.find(event.key.keysym.sym);
    if (it == mapping.end()) return;

    if (event.type == SDL_KEYUP) {
        delete pNextVelocity;
        pNextVelocity = nullptr;
        return;
    }

    pNextVelocity = new SDL_Point(it->second);
    mPrevDirection = it->second;
    initiateMove();
}

void Player::handleKeyboardEvent_ProjectileAttack(SDL_Event const& event) {
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

    PentacleProjectile::initiateAttack(it->second, mDestCoords, mPrevDirection);
}

void Player::handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const {
    if (mCurrAnimationType != AnimationType::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackRegister_Player_GHE);
    event::setData(event, event::data::Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

void Player::handleCustomEventPOST_kReq_Death_Player() const {
    if (mCurrAnimationType != AnimationType::kDeath) return;

    static int counter = config::entities::player::waitingFramesAfterDeath;
    if (counter > 0) --counter;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, counter == 0 ? event::Code::kReq_DeathFinalized_Player : event::Code::kReq_DeathPending_Player);
    event::setData(event, counter);
    event::enqueue(event);
}

void Player::handleCustomEventGET_kReq_AttackRegister_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::data::Generic>(event);

    if (mCurrAnimationType == AnimationType::kDamaged) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    EntitySecondaryStats::resolve(data.stats, mSecondaryStats);

    if (pNextAnimationType == nullptr) {
        pNextAnimationType = new AnimationType(mSecondaryStats.HP > 0 ? AnimationType::kDamaged : AnimationType::kDeath);
        mIsAnimationOnProgress = false;
    }
}

void Player::handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::data::Generic>(event);

    if (mCurrAnimationType == AnimationType::kDamaged || mCurrAnimationType == AnimationType::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, event::Code::kResp_AttackInitiate_GHE_Player);
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

void Player::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::data::Generic>(event);

    if (mCurrAnimationType == AnimationType::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, distance > data.range.x || distance > data.range.y ? event::Code::kResp_MoveTerminate_GHE_Player : event::Code::kResp_MoveInitiate_GHE_Player);
    data.destCoords = mDestCoords;
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

void Player::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) {
    auto data = event::getData<event::data::Teleporter>(event);

    if (mDestCoords != data.destCoords) return;

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, event::Code::kResp_Teleport_GTE_Player);
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}


template <>
int AbstractAnimatedDynamicEntity<Player>::sMoveDelay = config::entities::player::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Player>::sVelocity = config::entities::player::velocity;

template <>
const char* AbstractEntity<Player>::sTypeID = config::entities::player::typeID;

template <>
std::filesystem::path AbstractEntity<Player>::sTilesetPath = config::entities::player::path;

const std::vector<std::filesystem::path> Player::sTilesetPaths = config::entities::player::paths;