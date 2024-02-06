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
            if (event::getID(event) != id) return;
            handleCustomEventGET_kResp_AttackInitiate_GHE_Player();
            break;

        case event::Code::kResp_MoveInitiate_GHE_Player:
            if (event::getID(event) != id) return;
            handleCustomEventGET_kResp_MoveInitiate_GHE_Player(event);
            break;

        case event::Code::kResp_MoveTerminate_GHE_Player:
            if (event::getID(event) != id) return;
            handleCustomEventGET_kResp_MoveTerminate_GHE_Player();
            break;

        default: break;
    }
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackRegister_GHE_Player() const {
    if (currAnimationType != AnimationType::kAttack || !isAnimationAtFinalSprite()) return;

    auto event = event::instantiate();
    event::setID(event, id);
    event::setCode(event, event::Code::kReq_AttackRegister_GHE_Player);
    event::setData(event, event::data::Mob({ destCoords, kAttackRegisterRange, secondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackInitiate_GHE_Player() const {
    if (currAnimationType == AnimationType::kAttack) return;

    auto event = event::instantiate();
    event::setID(event, id);
    event::setCode(event, event::Code::kReq_AttackInitiate_GHE_Player);
    event::setData(event, event::data::Mob({ destCoords, kAttackInitiateRange, secondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_MoveInitiate_GHE_Player() const {
    auto event = event::instantiate();
    event::setID(event, id);
    event::setCode(event, event::Code::kReq_MoveInitiate_GHE_Player);
    event::setData(event, event::data::Mob({ destCoords, kMoveInitiateRange, secondaryStats }));
    event::enqueue(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kReq_AttackRegister_Player_GHE(SDL_Event const& event) {
    auto data = event::getData<event::data::Mob>(event);

    if (currAnimationType == AnimationType::kDamaged) return;
    auto distance = utils::calculateDistance(destCoords, data.destCoords);
    if (distance > data.range.x || distance > data.range.y) return;

    EntitySecondaryStats::resolve(data.stats, secondaryStats);

    if (nextAnimationType == nullptr) {
        nextAnimationType = new AnimationType(secondaryStats.HP > 0 ? AnimationType::kDamaged : AnimationType::kDeath);
        isAnimationOnProgress = false;
    }
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_AttackInitiate_GHE_Player() {
    if (nextAnimationType != nullptr) return;
    nextAnimationType = new AnimationType(AnimationType::kAttack);
    isAnimationOnProgress = false;
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = event::getData<event::data::Mob>(event);
    nextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (data.destCoords.x > destCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (data.destCoords.y > destCoords.y) * 2 - 1 });
    initiateMove();
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveTerminate_GHE_Player() {
    delete nextVelocity;
    nextVelocity = nullptr;
}


template class GenericHostileEntity<Slime>;