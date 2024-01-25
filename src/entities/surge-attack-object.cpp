#include <entities.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


SurgeAttackObject::SurgeAttackObject(SDL_Point const& destCoords, SDL_Point const& direction) : AbstractAnimatedDynamicEntity<SurgeAttackObject>(destCoords) {
    destRectModifier = config::entities::surge_attack_object::destRectModifier;
    kAttackRegisterRange = config::entities::surge_attack_object::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    primaryStats = config::entities::surge_attack_object::primaryStats;
    kDirection = direction;   // Cannot use initializer list

    resetAnimation(AnimationType::kAttack);
    onWindowChange();   // Must be manually called here to populate `destCoords`
}

void SurgeAttackObject::initialize() {
    AbstractAnimatedDynamicEntity<SurgeAttackObject>::initialize();
}

/**
 * @note Attacks should not persist beyond the scope of a level.
*/
void SurgeAttackObject::onLevelChangeAll() {
    Multiton<SurgeAttackObject>::deinitialize();
}

/**
 * @param destCoords handled by the entity from which the attack should be invoked. Usually calculated by `destCoords + *nextVelocity`.
*/
void SurgeAttackObject::initiateLinearAttack(SDL_Point const& destCoords, SDL_Point const& direction) {
    instantiate(destCoords, direction);
}

void SurgeAttackObject::initiateCircularAttack(SDL_Point const& destCoords) {
    // ...
}

void SurgeAttackObject::handleLifespan() {
    if (!isAnimationAtFinalSprite()) return;

    initiateNextLinearAttack();
    instances.erase(this);
}

void SurgeAttackObject::initiateNextLinearAttack() {
    nextDestCoords = new SDL_Point(destCoords + kDirection);
    if (!validateMove()) return;

    instantiate(*nextDestCoords, kDirection);
    Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kSurgeAttack);
}


template <>
int AbstractAnimatedDynamicEntity<SurgeAttackObject>::kMoveDelay = config::entities::surge_attack_object::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<SurgeAttackObject>::kVelocity = config::entities::surge_attack_object::velocity;

template <>
const std::filesystem::path AbstractEntity<SurgeAttackObject>::kTilesetPath = config::entities::surge_attack_object::path;