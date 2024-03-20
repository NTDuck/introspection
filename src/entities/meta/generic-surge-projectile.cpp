#include <entities.hpp>

#include <algorithm>
#include <string>
#include <stack>
#include <unordered_set>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @note Recommended implementation: derived classes should ensure a call to `onWindowChange()` after initializing `destRectModifier` to prevent undefined behaviour.
*/
template <typename T>
GenericSurgeProjectile<T>::GenericSurgeProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : AbstractAnimatedDynamicEntity<T>(destCoords) {
    mDirection = direction;
    resetAnimation(Animation::kAttackMeele);
    // onWindowChange();
}

/**
 * @note Attacks should not persist beyond the scope of a level.
*/
template <typename T>
void GenericSurgeProjectile<T>::onLevelChangeAll() {
    Multiton<T>::deinitialize();
}

template <typename T>
void GenericSurgeProjectile<T>::handleCustomEventPOST() const {
    handleCustomEventPOST_impl<event::Code::kReq_AttackRegister_Player_GHE>();
}

/**
 * @param destCoords the `destCoords` of the entity from which the attack would be invoked.
*/
template <typename T>
void GenericSurgeProjectile<T>::initiateAttack(ProjectileType type, SDL_Point const& destCoords, SDL_Point const& direction) {
    static constexpr SDL_Point kDiagonalQuadrupleDirection = { 1, 1 };

    switch (type) {
        case ProjectileType::kDiagonalQuadruple:
            initiateAttack(ProjectileType::kOrthogonalDouble, destCoords, kDiagonalQuadrupleDirection);
            initiateAttack(ProjectileType::kOrthogonalDouble, destCoords, kDiagonalQuadrupleDirection << 1);
            break;

        case ProjectileType::kOrthogonalQuadruple:
            initiateAttack(ProjectileType::kOrthogonalDouble, destCoords, direction);
            initiateAttack(ProjectileType::kOrthogonalDouble, destCoords, direction << 1);
            break;

        case ProjectileType::kOrthogonalTriple:
            initiateAttack(ProjectileType::kOrthogonalSingle, destCoords, direction);
            initiateAttack(ProjectileType::kOrthogonalDouble, destCoords, direction << 1);
            break;

        case ProjectileType::kOrthogonalDouble:
            initiateAttack(ProjectileType::kOrthogonalSingle, destCoords, -direction);
            [[fallthrough]];

        case ProjectileType::kOrthogonalSingle:   // Also work with diagonals
            instantiate(destCoords + direction, direction);   // Actual stuff
            break;

        default: break;
    }
}

template <typename T>
void GenericSurgeProjectile<T>::handleInstantiation() {
    if (!isAnimationAtFinalSprite()) return;

    initiateNextLinearAttack();
    sTerminatedInstances.push(reinterpret_cast<T*>(this));
}

template <typename T>
void GenericSurgeProjectile<T>::handleTermination() {
    while (!sTerminatedInstances.empty()) {
        delete sTerminatedInstances.top();
        sTerminatedInstances.pop();
    }
}

template <typename T>
void GenericSurgeProjectile<T>::initiateNextLinearAttack() {
    mNextDestCoords = new SDL_Point(mDestCoords + mDirection);
    if (!validateMove()) Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kSurgeAttack);
    else instantiate(*mNextDestCoords, mDirection);
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_AttackRegister_Player_GHE>
GenericSurgeProjectile<T>::handleCustomEventPOST_impl() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_AttackRegister_Player_GHE);
    event::setData(event, event::Data_Generic({ mDestCoords, mAttackRegisterRange, mSecondaryStats }));
    event::enqueue(event);
}


template <typename T>
std::stack<T*> GenericSurgeProjectile<T>::sTerminatedInstances;


template class GenericSurgeProjectile<PentacleProjectile>;


DEFINE_GENERIC_SURGE_PROJECTILE(PentacleProjectile, config::entities::pentacle_projectile)