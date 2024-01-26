#include <entities.hpp>

#include <SDL.h>

#include <auxiliaries.hpp>


HauntedBookcaseProjectile::HauntedBookcaseProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : GenericAerialProjectile<HauntedBookcaseProjectile>(destCoords, direction) {
    destRectModifier = config::entities::haunted_bookcase_projectile::destRectModifier;
    kAttackRegisterRange = config::entities::haunted_bookcase_projectile::attackRegisterRange;
    primaryStats = config::entities::haunted_bookcase_projectile::primaryStats;
}


template <>
int AbstractAnimatedDynamicEntity<HauntedBookcaseProjectile>::kMoveDelay = config::entities::haunted_bookcase_projectile::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<HauntedBookcaseProjectile>::kVelocity = config::entities::haunted_bookcase_projectile::velocity;

template <>
const std::filesystem::path AbstractEntity<HauntedBookcaseProjectile>::kTilesetPath = config::entities::haunted_bookcase_projectile::path;