#include <entities.hpp>

#include <algorithm>
#include <string>
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
void GenericSurgeProjectile<T>::handleLifespan() {
    if (!isAnimationAtFinalSprite()) return;

    initiateNextLinearAttack();
    instances.erase(dynamic_cast<T*>(this));
}

template <typename T>
void GenericSurgeProjectile<T>::initiateNextLinearAttack() {
    nextDestCoords = new SDL_Point(destCoords + kDirection);
    if (!validateMove()) Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kSurgeAttack);
    else instantiate(*nextDestCoords, kDirection);
}


template class GenericSurgeProjectile<PentacleProjectile>;