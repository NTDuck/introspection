#include <entities.hpp>

#include <filesystem>
#include <unordered_map>

#include <SDL.h>

#include <timers.hpp>
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
    static std::size_t index = 0;
    static const std::size_t size = sTilesetPaths.size();

    if (increment) { if (index == size - 1) index = 0; else ++index; } else { if (!index) index = size - 1; else --index; }

    AbstractAnimatedEntity<Player>::reinitialize(sTilesetPaths[index]);
}

void Player::onLevelChange(level::Data_Generic const& player) {
    auto data = *reinterpret_cast<const level::Data_Generic*>(&player);
    onAutopilotToggled(false);
    resetAnimation(Animation::kIdle, BehaviouralType::kPrioritized);
    AbstractAnimatedDynamicEntity<Player>::onLevelChange(data);
}

/**
 * @brief Calculate player movement based on keyboard input.
 * @note Generates `nextDestCoords` and `nextDestRect`.
*/
void Player::handleKeyboardEvent(SDL_Event const& event) {
    static CountdownTimer timer(config::entities::player::projectileCooldownTicks);
    if (!timer.isStarted()) timer.start();

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath) return;

    switch (event.key.keysym.sym) {
        case ~config::Key::kPlayerMoveUp:
        case ~config::Key::kPlayerMoveDown:
        case ~config::Key::kPlayerMoveRight:
        case ~config::Key::kPlayerMoveLeft:
            onAutopilotToggled(false);
            handleKeyboardEvent_Movement(event);
            break;

        case ~config::Key::kPlayerAutopilotToggle:
            if (event.type != SDL_KEYDOWN) break;
            onAutopilotToggled(!isOnAutopilot());
            break;

        case ~config::Key::kPlayerRunToggle:
            onRunningToggled(event.type == SDL_KEYDOWN);
            break;

        case ~config::Key::kPlayerAttackMeele:
            if (event.type != SDL_KEYDOWN) break;
            onAutopilotToggled(false);
            if (mAnimation == Animation::kAttackMeele || mAnimation == Animation::kAttackRanged) break;
            resetAnimation(Animation::kAttackMeele);
            break;

        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalSingle:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalDouble:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalTriple:
        case ~config::Key::kPlayerAttackSurgeProjectileOrthogonalQuadruple:
        case ~config::Key::kPlayerAttackSurgeProjectileDiagonalQuadruple:
            if (event.type != SDL_KEYDOWN) break;
            onAutopilotToggled(false);
            if (tile::Data_EntityTileset::getPriority(mAnimation) == tile::Data_EntityTileset::getPriority(Animation::kAttackMeele) || !timer.isFinished()) break;
            resetAnimation(Animation::kAttackRanged);

            handleKeyboardEvent_ProjectileAttack(event);
            timer.stop();
            break;

        case ~config::Key::kAffirmative:
            if (event.type != SDL_KEYDOWN) break;
            onAutopilotToggled(false);
            handleCustomEventPOST_impl<event::Code::kReq_Interact_Player_GIE>();
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
    handleCustomEventPOST_impl<event::Code::kReq_AttackRegister_Player_GHE>();
    handleCustomEventPOST_impl<event::Code::kReq_DeathFinalized_Player>();
}

void Player::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
        case event::Code::kReq_AttackRegister_GHE_Player:
            handleCustomEventGET_impl<event::Code::kReq_AttackRegister_GHE_Player>(event);
            break;

        case event::Code::kReq_AttackInitiate_GHE_Player:
            handleCustomEventGET_impl<event::Code::kResp_AttackInitiate_GHE_Player>(event);
            break;

        case event::Code::kReq_MoveInitiate_GHE_Player:
            handleCustomEventGET_impl<event::Code::kResp_MoveInitiate_GHE_Player>(event);
            break;

        case event::Code::kReq_Teleport_GTE_Player:
            handleCustomEventGET_impl<event::Code::kResp_Teleport_GTE_Player>(event);
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

    switch (mAnimation) {
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

/**
 * @brief Calculate path to `level::data.autopilotTargetTile`.
*/
void Player::onAutopilotToggled(bool onAutopilotStart) {
    if (!onAutopilotStart) {
        Umbra::instantiateEX({});
        while (!mAutopilotPath.empty()) mAutopilotPath.pop();
        if (mNextVelocity != nullptr) delete mNextVelocity;
        mNextVelocity = nullptr;
        return;
    }

    static auto pathfinder = pathfinders::ASPF<pathfinders::Heuristic::kManhattan, pathfinders::MovementType::k4Directional>(level::data.collisionTilelayer);
    pathfinder.setBegin({ 0, 0 });
    pathfinder.setEnd({ static_cast<int>(level::data.collisionTilelayer.front().size()) - 1, static_cast<int>(level::data.collisionTilelayer.size()) - 1 });

    auto result = pathfinder.search(pathfinders::Cell::pttocl(mDestCoords), pathfinders::Cell::pttocl(level::data.autopilotTargetTile));
    if (result.status != pathfinders::Status::kSuccess) return;

    mAutopilotPath = result.path;
    
    std::vector<level::Data_Generic*> umbraLevelData(result.path.size());
    for (auto& data : umbraLevelData) {
        data = new level::Data_Generic(pathfinders::Cell::cltopt(result.path.top()));
        result.path.pop();
    }
    Umbra::instantiateEX(umbraLevelData);
}

/**
 * @brief Determine player movement on autopilot.
*/
void Player::handleAutopilotMovement() {
    if (!isOnAutopilot() || mDestCoords != pathfinders::Cell::cltopt(mAutopilotPath.top())) return;

    // The current topmost element of the path is the player's current position, so remove it
    mAutopilotPath.pop();
    if (mAutopilotPath.empty()) return;

    onMoveEnd(BehaviouralType::kAutopilot);   // This took more than 1 week to debug

    // After removal, the current topmost element of the path is the player's (supposedly) next position, so base calculations on it
    // if (mNextVelocity != nullptr) delete mNextVelocity;
    mNextVelocity = new SDL_Point(pathfinders::Cell::cltopt(mAutopilotPath.top()) - mDestCoords);
    initiateMove(BehaviouralType::kAutopilot);
}

void Player::handleKeyboardEvent_Movement(SDL_Event const& event) {
    static const std::unordered_map<SDL_Keycode, SDL_Point> mapping = {
        { ~config::Key::kPlayerMoveUp, { 0, -1 } },
        { ~config::Key::kPlayerMoveDown, { 0, 1 } },
        { ~config::Key::kPlayerMoveRight, { -1, 0 } },
        { ~config::Key::kPlayerMoveLeft, { 1, 0 } },
    };

    auto it = mapping.find(event.key.keysym.sym);
    if (it == mapping.end()) return;

    delete mNextVelocity;
    mNextVelocity = nullptr;

    if (event.type == SDL_KEYDOWN) {
        mNextVelocity = new SDL_Point(it->second);
        initiateMove();
    }
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

    Darkness::initiateAttack(it->second, mDestCoords, mDirection);
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_Player_GHE>
Player::handleCustomEventPOST_impl() const {
    if (mAnimation != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackRegister_Player_GHE);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_DeathFinalized_Player || C == event::Code::kReq_DeathPending_Player>
Player::handleCustomEventPOST_impl() const {
    if (mAnimation != Animation::kDeath) return;

    static CountdownTimer timer(config::entities::player::deathTicks);
    if (!timer.isStarted()) timer.start();

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, timer.isFinished() ? event::Code::kReq_DeathFinalized_Player : event::Code::kReq_DeathPending_Player);
    event::setData(event, true);
    event::enqueue(event);

    if (timer.isFinished()) timer.stop();
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_Interact_Player_GIE>
Player::handleCustomEventPOST_impl() const {
    if (mAnimation == Animation::kDeath) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_Interact_Player_GIE);
    event::setData(event, event::Data_Interactable({ mDestCoords + mDirection }));
    event::enqueue(event);
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_GHE_Player>
Player::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath) return;

    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    EntitySecondaryStats::resolve(data.stats, mSecondaryStats);

    if (mSecondaryStats.HP > 0) {
        resetAnimation(Animation::kDamaged);
        Slash::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
    } else {
        resetAnimation(Animation::kDeath);
        Claw::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
    }
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_AttackInitiate_GHE_Player>
Player::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath) return;

    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, event::Code::kResp_AttackInitiate_GHE_Player);
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_MoveInitiate_GHE_Player>
Player::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDeath) return;

    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, distance > data.range.x || distance > data.range.y ? event::Code::kResp_MoveTerminate_GHE_Player : event::Code::kResp_MoveInitiate_GHE_Player);
    data.destCoords = mDestCoords;
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_Teleport_GTE_Player>
Player::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Teleporter>(event);

    if (mDestCoords != data.destCoords) return;

    auto followupEvent = event::instantiate();
    event::setID(followupEvent, event::getID(event));
    event::setCode(followupEvent, event::Code::kResp_Teleport_GTE_Player);
    event::setData(followupEvent, data);
    event::enqueue(followupEvent);
}


template <>
const unsigned int AbstractAnimatedDynamicEntity<Player>::sMoveDelayTicks = config::entities::player::moveDelayTicks;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Player>::sVelocity = config::entities::player::velocity;

template <>
const char* AbstractEntity<Player>::sTypeID = config::entities::player::typeID;

template <>
std::filesystem::path AbstractEntity<Player>::sTilesetPath = config::entities::player::path;

const std::vector<std::filesystem::path> Player::sTilesetPaths = config::entities::player::paths;