#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericHostileEntity<T>::GenericHostileEntity(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<T>(destCoords) {}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST() const {
    handleCustomEventPOST_impl<event::Code::kReq_AttackRegister_GHE_Player>();
    handleCustomEventPOST_impl<event::Code::kReq_AttackInitiate_GHE_Player>();
    handleCustomEventPOST_impl<event::Code::kReq_MoveInitiate_GHE_Player>();
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET(SDL_Event const& event) {
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

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_GHE_Player>
GenericHostileEntity<T>::handleCustomEventPOST_impl() const {
    if (mAnimation != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackInitiate_GHE_Player>
GenericHostileEntity<T>::handleCustomEventPOST_impl() const {
    if (mAnimation == Animation::kAttackMeele) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_MoveInitiate_GHE_Player>
GenericHostileEntity<T>::handleCustomEventPOST_impl() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Generic({ mDestCoords, mMoveInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_Player_GHE>
GenericHostileEntity<T>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);

    if (mAnimation == Animation::kDamaged || mAnimation == Animation::kDeath) return;
    auto distance = utils::calculateDistance(mDestCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    bool isDeadPrior = mSecondaryStats.HP <= 0;
    EntitySecondaryStats::resolve(data.stats, mSecondaryStats);
    if (!isDeadPrior && mSecondaryStats.HP <= 0) ++sDeathCount;

    resetAnimation(mSecondaryStats.HP > 0 ? Animation::kDamaged : Animation::kDeath);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_AttackInitiate_GHE_Player>
GenericHostileEntity<T>::handleCustomEventGET_impl() {
    resetAnimation(Animation::kAttackMeele);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_MoveInitiate_GHE_Player>
GenericHostileEntity<T>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);
    mNextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (data.destCoords.x > mDestCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (data.destCoords.y > mDestCoords.y) * 2 - 1 });
    initiateMove();
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_MoveTerminate_GHE_Player>
GenericHostileEntity<T>::handleCustomEventGET_impl() {
    delete mNextVelocity;
    mNextVelocity = nullptr;
}


template <typename T>
unsigned int GenericHostileEntity<T>::sDeathCount = 0;


template class GenericHostileEntity<Slime>;


DEFINE_GENERIC_HOSTILE_ENTITY(Slime, config::entities::slime)