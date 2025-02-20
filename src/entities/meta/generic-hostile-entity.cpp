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
void GenericHostileEntity<T, M>::instantiateMeteorProjectileOnSelf() {
    Meteor::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventPOST_impl() const {
    if (mAnimation != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, &mAttributes }));
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
    event::setData(event, event::Data_Generic({ mDestCoords, &mAttributes }));
    event::enqueue(event);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_MoveInitiate_GHE_Player>
GenericHostileEntity<T, M>::handleCustomEventPOST_impl() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, &mAttributes }));
    event::enqueue(event);
}

template <typename T, MovementSelectionType M>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_Player_GHE>
GenericHostileEntity<T, M>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath || !data.attributes->within<EntityAttributes::ID::ARR>(mDestCoords, data.destCoords)) return;

    if (data.attributes->attack(mAttributes)) {
        ++sDeathCount;
        resetAnimation(Animation::kDeath);
        Claw::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
    } else {
        resetAnimation(Animation::kDamaged);
        Slash::initiateAttack(ProjectileType::kOrthogonalSingle, mDestCoords, { 0, 0 });
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
    static auto dir = [](SDL_Point const& endpoint, SDL_Point const& origin) -> SDL_Point {
        auto endpoint_shifted = (endpoint - origin) >> (M_PI / 4);   // 45 degrees counterclockwise rotation
        if (endpoint_shifted.x >= 0 && endpoint_shifted.y >= 0) return { 0, 1 };
        if (endpoint_shifted.x >= 0 && endpoint_shifted.y <= 0) return { 1, 0 };
        if (endpoint_shifted.x <= 0 && endpoint_shifted.y <= 0) return { 0, -1 };
        if (endpoint_shifted.x <= 0 && endpoint_shifted.y >= 0) return { -1, 0 };
        return tile::Data_EntityTileset::kDefaultDirection;
    };

    mNextVelocity = new SDL_Point(dir(targetDestCoords, mDestCoords));
}

template <typename T, MovementSelectionType M>
template <MovementSelectionType M_>
typename std::enable_if_t<M_ == MovementSelectionType::kGreedyRandomBinary>
GenericHostileEntity<T, M>::calculateNextMovement(SDL_Point const& targetDestCoords) {
    mNextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (targetDestCoords.x > mDestCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (targetDestCoords.y > mDestCoords.y) * 2 - 1 });
}

/**
 * @warning Bugged, do not use this.
*/
template <typename T, MovementSelectionType M>
template <MovementSelectionType M_>
typename std::enable_if_t<M_ == MovementSelectionType::kPathfindingAStar>
GenericHostileEntity<T, M>::calculateNextMovement(SDL_Point const& targetDestCoords) {
    static auto pathfinder = pathfinders::ASPF<pathfinders::Heuristic::kManhattan, pathfinders::MovementType::k4Directional>(level::data.collisionTilelayer);

    static auto timer = CountdownTimer(config::entities::ASPFTicks);
    if (!timer.isStarted()) timer.start();
    if (!timer.isFinished()) return;

    pathfinder.setBegin(pathfinders::Cell::pttocl(mDestCoords - mAttributes.template get<EntityAttributes::ID::MIR>()));
    pathfinder.setEnd(pathfinders::Cell::pttocl(mDestCoords + mAttributes.template get<EntityAttributes::ID::MIR>()));

    auto result = pathfinder.search(pathfinders::Cell::pttocl(mDestCoords), pathfinders::Cell::pttocl(targetDestCoords));
    if (result.status != pathfinders::Status::kSuccess || result.path.empty()) return;

    result.path.pop();
    if (result.path.empty()) return;

    mNextVelocity = new SDL_Point( pathfinders::Cell::cltopt(result.path.top()) - mDestCoords );

    timer.start();
}


template <typename T, MovementSelectionType M>
unsigned int GenericHostileEntity<T, M>::sDeathCount = 0;


template class GenericHostileEntity<CommonHooded>;
template class GenericHostileEntity<CommonSatyr>;
template class GenericHostileEntity<CommonChickenBoy>;
template class GenericHostileEntity<ElitePlagueCrow>;
template class GenericHostileEntity<EliteDarkSamurai>;
template class GenericHostileEntity<BossRedHood>;
template class GenericHostileEntity<BossNightBorne>;
template class GenericHostileEntity<Egg>;
template class GenericHostileEntity<EnergySphere>;


DEF_GENERIC_HOSTILE_ENTITY_(ElitePlagueCrow, config::entities::hostile::elite_plague_crow)
DEF_GENERIC_HOSTILE_ENTITY_(EliteDarkSamurai, config::entities::hostile::elite_dark_samurai)
DEF_GENERIC_HOSTILE_ENTITY_(CommonHooded, config::entities::hostile::common_hooded)
DEF_GENERIC_HOSTILE_ENTITY_(CommonSatyr, config::entities::hostile::common_satyr)
DEF_GENERIC_HOSTILE_ENTITY_(CommonChickenBoy, config::entities::hostile::common_chicken_boy)
DEF_GENERIC_HOSTILE_ENTITY_(BossRedHood, config::entities::hostile::boss_red_hood)
DEF_GENERIC_HOSTILE_ENTITY_(BossNightBorne, config::entities::hostile::boos_night_borne)
DEF_GENERIC_HOSTILE_ENTITY_(Egg, config::entities::hostile::egg)
DEF_GENERIC_HOSTILE_ENTITY_(EnergySphere, config::entities::hostile::energy_sphere)