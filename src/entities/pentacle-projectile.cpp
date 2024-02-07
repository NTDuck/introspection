#include <entities.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


PentacleProjectile::PentacleProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : GenericSurgeProjectile<PentacleProjectile>(destCoords, direction) {
    mDestRectModifier = config::entities::pentacle_projectile::destRectModifier;
    mAttackRegisterRange = config::entities::pentacle_projectile::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    mPrimaryStats = config::entities::pentacle_projectile::primaryStats;

    onWindowChange();
}


template <>
int AbstractAnimatedDynamicEntity<PentacleProjectile>::sMoveDelay = config::entities::pentacle_projectile::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<PentacleProjectile>::sVelocity = config::entities::pentacle_projectile::velocity;

template <>
const std::filesystem::path AbstractEntity<PentacleProjectile>::kTilesetPath = config::entities::pentacle_projectile::path;