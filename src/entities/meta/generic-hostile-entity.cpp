#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericHostileEntity<T>::GenericHostileEntity(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<T>(destCoords) {}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST() const {
    handleCustomEventPOST_kReq_AttackRegister_GHE_Player();
    handleCustomEventPOST_kReq_AttackInitiate_GHE_Player();
    handleCustomEventPOST_kReq_MoveInitiate_GHE_Player();
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
        case event::Code::kReq_AttackRegister_Player_GHE:
            handleCustomEventGET_kReq_AttackRegister_Player_GHE(event);
            break;

        case event::Code::kResp_AttackInitiate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_kResp_AttackInitiate_GHE_Player();
            break;

        case event::Code::kResp_MoveInitiate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_kResp_MoveInitiate_GHE_Player(event);
            break;

        case event::Code::kResp_MoveTerminate_GHE_Player:
            if (event::getID(event) != mID) return;
            handleCustomEventGET_kResp_MoveTerminate_GHE_Player();
            break;

        default: break;
    }
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackRegister_GHE_Player() const {
    if (mCurrAnimationType != Animation::kAttackMeele || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackRegister_GHE_Player);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackInitiate_GHE_Player() const {
    if (mCurrAnimationType == Animation::kAttackMeele) return;

    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackInitiate_GHE_Player);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_MoveInitiate_GHE_Player() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_MoveInitiate_GHE_Player);
    event::setData(event, event::Data_Generic({ mDestCoords, mMoveInitiateRange, mSecondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kReq_AttackRegister_Player_GHE(SDL_Event const& event) {
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

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_AttackInitiate_GHE_Player() {
    if (pNextAnimationType != nullptr) return;
    pNextAnimationType = new Animation(Animation::kAttackMeele);
    mIsAnimationOnProgress = false;
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::Data_Generic>(event);
    pNextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (data.destCoords.x > mDestCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (data.destCoords.y > mDestCoords.y) * 2 - 1 });
    initiateMove();
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveTerminate_GHE_Player() {
    delete pNextVelocity;
    pNextVelocity = nullptr;
}


template class GenericHostileEntity<Slime>;


DEFINE_GENERIC_HOSTILE_ENTITY(Slime, config::entities::slime)