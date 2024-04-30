#include <entities.hpp>

#include <chrono>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T, MovementSelectionType M>
GenericHostileEntity<T, M>::GenericHostileEntity(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<T>(destCoords) {}

template <typename T, MovementSelectionType M>
void GenericHostileEntity<T, M>::handleCustomEventPOST() const {
    handleCustomEventPOST_impl<event::Code::kReq_AttackRegister_GHE_Player>();
    handleCustomEventPOST_impl<event::Code::kReq_AttackInitiate_GHE_Player>();
    handleCustomEventPOST_impl<event::Code::kReq_MoveInitiate_GHE_Player>();
}

template <typename T, MovementSelectionType M>
void GenericHostileEntity<T, M>::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
        case event::Code::kReq_AttackRegister_Player_GHE:
            handleCustomEventGET_impl<event::Code::kReq_AttackRegister_Player_GHE>(event);
            break;

        case event::Code::kResp_AttackInitiate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_impl<event::Code::kResp_AttackInitiate_GHE_Player>();
            break;

        case event::Code::kResp_MoveInitiate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_impl<event::Code::kResp_MoveInitiate_GHE_Player>(event);
            break;

        case event::Code::kResp_MoveTerminate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_impl<event::Code::kResp_MoveTerminate_GHE_Player>();
            break;

        default: break;
    }
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventPOST_impl() const {
    if (mAnimation != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackInitiate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventPOST_impl() const {
    if (mAnimation == Animation::kAttackMeele) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_MoveInitiate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventPOST_impl() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mMoveInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_Player_GHE>
GenericHostileEntity<T, M>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    bool isDeadPrior = mSecondaryStats.HP <= 0;
    EntitySecondaryStats::resolve(data.stats, mSecondaryStats);
    if (!isDeadPrior && mSecondaryStats.HP <= 0) ++sDeathCount;

    if (mSecondaryStats.HP > 0) {
        resetAnimation(Animation::kDamaged);
        Slash::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
    } else {
        resetAnimation(Animation::kDeath);
        Claw::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
    }
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_AttackInitiate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventGET_impl() {
    resetAnimation(Animation::kAttackMeele);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_MoveInitiate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);
    calculateNextMovement(data.destCoords);
    initiateMove();
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_MoveTerminate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventGET_impl() {
    delete mNextVelocity;
    mNextVelocity = nullptr;
}

template <typename T, MovementSelectionType M>
template <MovementSelectionType M_>
typename std::enable_if_t<M_ == MovementSelectionType::kGreedyTrigonometric>
GenericHostileEntity<T, M>::calculateNextMovement(SDL_Point const& targetDestCoords) {
    mNextVelocity = new SDL_Point(
        utils::calculateDirection(targetDestCoords, mDestCoords)
    );
}

template <typename T, MovementSelectionType M>
template <MovementSelectionType M_>
typename std::enable_if_t<M_ == MovementSelectionType::kGreedyRandomBinary>
GenericHostileEntity<T, M>::calculateNextMovement(SDL_Point const& targetDestCoords) {
    mNextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (targetDestCoords.x > mDestCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (targetDestCoords.y > mDestCoords.y) * 2 - 1 });
}

template <typename T, MovementSelectionType M>
template <MovementSelectionType M_>
typename std::enable_if_t<M_ == MovementSelectionType::kPathfindingAStar>
GenericHostileEntity<T, M>::calculateNextMovement(SDL_Point const& targetDestCoords) {
    static auto pathfinder = pathfinders::ASPF<pathfinders::Heuristic::kManhattan, pathfinders::MovementType::k4Directional>(level::data.collisionTilelayer);

    static auto timer = CountdownTimer(config::entities::ASPFTicks);
    if (!timer.isStarted()) timer.start();
    if (!timer.isFinished()) return;

    pathfinder.setBegin(pathfinders::Cell::pttocl(mDestCoords - mMoveInitiateRange));
    pathfinder.setEnd(pathfinders::Cell::pttocl(mDestCoords + mMoveInitiateRange));

    auto result = pathfinder.search(pathfinders::Cell::pttocl(mDestCoords), pathfinders::Cell::pttocl(targetDestCoords));
    if (result.status != pathfinders::Status::kSuccess || result.path.empty()) return;

    result.path.pop();
    if (result.path.empty()) return;

    mNextVelocity = new SDL_Point( pathfinders::Cell::cltopt(result.path.top()) - mDestCoords );

    timer.start();
}


template <typename T, MovementSelectionType M>
unsigned int GenericHostileEntity<T, M>::sDeathCount = 0;


template class GenericHostileEntity<Dummy>;


DEF_GENERIC_HOSTILE_ENTITY_(Dummy, config::entities::hostile::dog)