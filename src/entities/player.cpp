#include <entities.hpp>

#include <filesystem>
#include <unordered_map>

#include <SDL.h>

#include <mixer.hpp>
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
    sTilesetData.clear();
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
    static constexpr unsigned short int count_limit = config::game::FPS >> 2;
    static unsigned short int count = 0;
    if (count > 0) --count;

    if (mCurrAnimationType == Animation::kDamaged || (pNextAnimationType != nullptr && *pNextAnimationType == Animation::kDamaged)) return;
    if (mCurrAnimationType == Animation::kDeath || (pNextAnimationType != nullptr && *pNextAnimationType == Animation::kDeath)) return;

    switch (event.key.keysym.sym) {
        case ~config::Key::kPlayerMoveUp:
        case ~config::Key::kPlayerMoveDown:
        case ~config::Key::kPlayerMoveRight:
        case ~config::Key::kPlayerMoveLeft:
            handleKeyboardEvent_Movement(event);
            break;

        case ~config::Key::kPlayerRunToggle:
            onRunningToggled(event.type == SDL_KEYDOWN);
            break;

        case ~config::Key::kPlayerAttackMeele:
            if (mCurrAnimationType == Animation::kAttackMeele || mCurrAnimationType == Animation::kAttackRanged) break;
            resetAnimation(Animation::kAttackMeele);
            break;

        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalSingle:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalDouble:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalTriple:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalQuadruple:
        case ~config::Key::kPlayerAttackSurgeProjectileDiagonalQuadruple:
            if (mCurrAnimationType == Animation::kAttackMeele || mCurrAnimationType == Animation::kAttackRanged) break;
            resetAnimation(Animation::kAttackRanged);

            if (count) break;
            handleKeyboardEvent_ProjectileAttack(event);
            count = count_limit;
            break;

        case ~config::Key::kAffirmative:
            handleCustomEventPOST_kReq_Interact_Player_GIE();
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

void Player::handleSFX() const {
    AbstractAnimatedDynamicEntity<Player>::handleSFX();

    static constexpr int idleFrames = config::game::FPS >> 2;   // Magic number
    static int idleFramesTracker = idleFrames;

    if (idleFramesTracker) {
        --idleFramesTracker;
        return;
    }

    switch (mCurrAnimationType) {
        case Animation::kWalk:
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kPlayerWalk);
            break;

        case Animation::kRun:
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kPlayerRun);
            break;
        
        default: return;
    }

    idleFramesTracker = idleFrames;
}

void Player::handleKeyboardEvent_Movement(SDL_Event const& event) {
    static const std::unordered_map<SDL_Keycode, SDL_Point> mapping = {
        { ~config::Key::kPlayerMoveUp, {0, -1} },
        { ~config::Key::kPlayerMoveDown, {0, 1} },
        { ~config::Key::kPlayerMoveRight, {-1, 0} },
        { ~config::Key::kPlayerMoveLeft, {1, 0} },
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
        { ~config::Key::kPlayerAttackSurgeProjectileOrthogonalSingle, ProjectileType::kOrthogonalSingle },
        { ~config::Key::kPlayerAttackSurgeProjectileOrthogonalDouble, ProjectileType::kOrthogonalDouble },
        { ~config::Key::kPlayerAttackSurgeProjectileOrthogonalTriple, ProjectileType::kOrthogonalTriple },
        { ~config::Key::kPlayerAttackSurgeProjectileOrthogonalQuadruple, ProjectileType::kOrthogonalQuadruple },
        { ~config::Key::kPlayerAttackSurgeProjectileDiagonalQuadruple, ProjectileType::kDiagonalQuadruple },
    };

    auto it = mapping.find(event.key.keysym.sym);
    if (it == mapping.end()) return;

    if (event.type == SDL_KEYUP) return;

    PentacleProjectile::initiateAttack(it->second, mDestCoords, mPrevDirection);
}

void Player::handleCustomEventPOST_kReq_AttackRegister_Player_GHE() const {
    if (mCurrAnimationType != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackRegister_Player_GHE);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

void Player::handleCustomEventPOST_kReq_Death_Player() const {
    if (mCurrAnimationType != Animation::kDeath) return;

    static int counter = config::entities::player::waitingFramesAfterDeath;
    if (counter > 0) --counter;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, counter == 0 ? event::Code::kReq_DeathFinalized_Player : event::Code::kReq_DeathPending_Player);
    event::setData(event, counter);
    event::enqueue(event);
}

void Player::handleCustomEventPOST_kReq_Interact_Player_GIE() const {
    if (mCurrAnimationType == Animation::kDeath) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_Interact_Player_GIE);
    event::setData(event, event::Data_Interactable({ mDestCoords + mPrevDirection }));
    event::enqueue(event);
}

void Player::handleCustomEventGET_kReq_AttackRegister_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mCurrAnimationType == Animation::kDamaged) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    EntitySecondaryStats::resolve(data.stats, mSecondaryStats);

    if (pNextAnimationType == nullptr) {
        pNextAnimationType = new Animation(mSecondaryStats.HP > 0 ? Animation::kDamaged : Animation::kDeath);
        mIsAnimationOnProgress = false;
    }
}

void Player::handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mCurrAnimationType == Animation::kDamaged || mCurrAnimationType == Animation::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, event::Code::kResp_AttackInitiate_GHE_Player);
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

void Player::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mCurrAnimationType == Animation::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, distance > data.range.x || distance > data.range.y ? event::Code::kResp_MoveTerminate_GHE_Player : event::Code::kResp_MoveInitiate_GHE_Player);
    data.destCoords = mDestCoords;
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

void Player::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) {
    auto data = event::getData<event::Data_Teleporter>(event);

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