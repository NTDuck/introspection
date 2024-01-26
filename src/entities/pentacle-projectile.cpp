#include <entities.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


PentacleProjectile::PentacleProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : GenericSurgeProjectile<PentacleProjectile>(destCoords, direction) {
    destRectModifier = config::entities::pentacle_projectile::destRectModifier;
    kAttackRegisterRange = config::entities::pentacle_projectile::attackRegisterRange;   // `kAttackInitiateRange` is unnecessary
    primaryStats = config::entities::pentacle_projectile::primaryStats;

    onWindowChange();
}


template <>
int AbstractAnimatedDynamicEntity<PentacleProjectile>::kMoveDelay = config::entities::pentacle_projectile::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<PentacleProjectile>::kVelocity = config::entities::pentacle_projectile::velocity;

template <>
const std::filesystem::path AbstractEntity<PentacleProjectile>::kTilesetPath = config::entities::pentacle_projectile::path;