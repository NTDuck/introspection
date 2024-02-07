// #include <entities.hpp>

// #include <SDL.h>

// #include <auxiliaries.hpp>


// HauntedBookcaseProjectile::HauntedBookcaseProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : GenericAerialProjectile<HauntedBookcaseProjectile>(destCoords, direction) {
//     mDestRectModifier = config::entities::haunted_bookcase_projectile::destRectModifier;
//     mAttackRegisterRange = config::entities::haunted_bookcase_projectile::attackRegisterRange;
//     mPrimaryStats = config::entities::haunted_bookcase_projectile::primaryStats;
// }


// template <>
// int AbstractAnimatedDynamicEntity<HauntedBookcaseProjectile>::sMoveDelay = config::entities::haunted_bookcase_projectile::moveDelay;

// template <>
// SDL_FPoint AbstractAnimatedDynamicEntity<HauntedBookcaseProjectile>::sVelocity = config::entities::haunted_bookcase_projectile::velocity;

// template <>
// const std::filesystem::path AbstractEntity<HauntedBookcaseProjectile>::kTilesetPath = config::entities::haunted_bookcase_projectile::path;