#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Slime::Slime(SDL_Point const& destCoords) : GenericHostileEntity<Slime>(destCoords) {
    mDestRectModifier = config::entities::slime::destRectModifier;
    mMoveInitiateRange = config::entities::slime::moveInitiateRange;
    mAttackInitiateRange = config::entities::slime::attackInitiateRange;
    mAttackRegisterRange = config::entities::slime::attackRegisterRange;
    mPrimaryStats = config::entities::slime::primaryStats;
}


template <>
int AbstractAnimatedDynamicEntity<Slime>::sMoveDelay = config::entities::slime::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::sVelocity = config::entities::slime::velocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = config::entities::slime::path;