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
    switch (static_cast<event::Code>(event.user.code)) {
        case event::Code::kReq_AttackRegister_Player_GHE:
            handleCustomEventGET_kReq_AttackRegister_Player_GHE(event);
            break;

        case event::Code::kResp_AttackInitiate_GHE_Player:
            if (*reinterpret_cast<int*>(event.user.data2) != id) return;
            handleCustomEventGET_kResp_AttackInitiate_GHE_Player(event);
            break;

        case event::Code::kResp_MoveInitiate_GHE_Player:
            if (*reinterpret_cast<int*>(event.user.data2) != id) return;
            handleCustomEventGET_kResp_MoveInitiate_GHE_Player(event);
            break;

        case event::Code::kResp_MoveTerminate_GHE_Player:
            if (*reinterpret_cast<int*>(event.user.data2) != id) return;
            handleCustomEventGET_kResp_MoveTerminate_GHE_Player(event);
            break;

        default: break;
    }
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackRegister_GHE_Player() const {
    if (currAnimationType != AnimationType::kAttack || !isAnimationAtFinalSprite()) return;

    auto event = formatCustomEvent();
    populateCustomEvent(event, event::Code::kReq_AttackRegister_GHE_Player, event::data::kReq_AttackRegister_GHE_Player({ destCoords, kAttackRegisterRange, secondaryStats }));
    enqueueCustomEvent(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_AttackInitiate_GHE_Player() const {
    if (currAnimationType == AnimationType::kAttack) return;

    auto event = formatCustomEvent();
    populateCustomEvent(event, event::Code::kReq_AttackInitiate_GHE_Player, event::data::kReq_AttackInitiate_GHE_Player({ destCoords, kAttackInitiateRange, secondaryStats }));
    enqueueCustomEvent(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventPOST_kReq_MoveInitiate_GHE_Player() const {
    auto event = formatCustomEvent();
    populateCustomEvent(event, event::Code::kReq_MoveInitiate_GHE_Player, event::data::kReq_MoveInitiate_GHE_Player({ destCoords, kMoveInitiateRange, secondaryStats }));
    enqueueCustomEvent(event);
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kReq_AttackRegister_Player_GHE(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_AttackRegister_Player_GHE*>(event.user.data1);

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

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_AttackInitiate_GHE_Player(SDL_Event const& event) {
    if (nextAnimationType != nullptr) return;
    nextAnimationType = new AnimationType(AnimationType::kAttack);
    isAnimationOnProgress = false;
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveInitiate_GHE_Player(SDL_Event const& event) {
    auto data = *reinterpret_cast<event::data::kReq_MoveInitiate_GHE_Player*>(event.user.data1);
    nextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (data.destCoords.x > destCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (data.destCoords.y > destCoords.y) * 2 - 1 });
    initiateMove();
}

template <typename T>
void GenericHostileEntity<T>::handleCustomEventGET_kResp_MoveTerminate_GHE_Player(SDL_Event const& event) {
    delete nextVelocity;
    nextVelocity = nullptr;
}


template class GenericHostileEntity<Slime>;