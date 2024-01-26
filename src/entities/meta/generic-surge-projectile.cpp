#include <entities.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @note Recommended implementation: derived classes should ensure a call to `onWindowChange()` after initializing `destRectModifier` to prevent undefined behaviour.
*/
template <typename T>
GenericSurgeProjectile<T>::GenericSurgeProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : AbstractAnimatedDynamicEntity<T>(destCoords) {
    kDirection = direction;

    resetAnimation(AnimationType::kAttack);
    // onWindowChange();
}

/**
 * @note Attacks should not persist beyond the scope of a level.
*/
template <typename T>
void GenericSurgeProjectile<T>::onLevelChangeAll() {
    Multiton<T>::deinitialize();
}

/**
 * @param destCoords handled by the entity from which the attack should be invoked. Usually calculated by `destCoords + *nextVelocity`.
*/
template <typename T>
void GenericSurgeProjectile<T>::initiateLinearAttack(SDL_Point const& destCoords, SDL_Point const& direction) {
    instantiate(destCoords, direction);
}

template <typename T>
void GenericSurgeProjectile<T>::initiateCircularAttack(SDL_Point const& destCoords) {
    // ...
}

template <typename T>
void GenericSurgeProjectile<T>::handleLifespan() {
    if (!isAnimationAtFinalSprite()) return;

    initiateNextLinearAttack();
    instances.erase(dynamic_cast<T*>(this));
}

template <typename T>
void GenericSurgeProjectile<T>::initiateNextLinearAttack() {
    nextDestCoords = new SDL_Point(destCoords + kDirection);
    if (!validateMove()) return;

    instantiate(*nextDestCoords, kDirection);
    Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kSurgeAttack);
}


template class GenericSurgeProjectile<PentacleProjectile>;