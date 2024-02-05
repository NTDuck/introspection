#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Slime::Slime(SDL_Point const& destCoords) : GenericHostileEntity<Slime>(destCoords) {
    destRectModifier = config::entities::slime::destRectModifier;
    kMoveInitiateRange = config::entities::slime::moveInitiateRange;
    kAttackInitiateRange = config::entities::slime::attackInitiateRange;
    kAttackRegisterRange = config::entities::slime::attackRegisterRange;
    primaryStats = config::entities::slime::primaryStats;
}


template <>
int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = config::entities::slime::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = config::entities::slime::velocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = config::entities::slime::path;