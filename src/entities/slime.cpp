#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Slime::Slime(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<Slime>(destCoords) {
    destRectModifier = config::entities::slime::destRectModifier;
    kMoveInitiateRange = config::entities::slime::moveInitiateRange;
    kAttackInitiateRange = config::entities::slime::attackInitiateRange;
    kAttackRegisterRange = config::entities::slime::attackRegisterRange;
    primaryStats = config::entities::slime::primaryStats;
}

/**
 * @brief Calculate the movement of an instance based on the position of the player entity.
 * @note Use `<cstdlib>` instead of `<random>` for slight performance gains. (sacrifice crypt)
*/
void Slime::calculateMove(SDL_Point const& playerDestCoords) {
    auto distance = utils::calculateDistance(destCoords, playerDestCoords);
    if (kMoveInitiateRange.x < distance && kMoveInitiateRange.y < distance) {
        delete nextVelocity;
        nextVelocity = nullptr;
        return;
    }

    nextVelocity = utils::generateRandomBinary() ? new SDL_Point({ (playerDestCoords.x > destCoords.x) * 2 - 1, 0 }) : new SDL_Point({ 0, (playerDestCoords.y > destCoords.y) * 2 - 1 });
    AbstractAnimatedDynamicEntity<Slime>::initiateMove();
}

template <>
int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = config::entities::slime::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = config::entities::slime::velocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = config::entities::slime::path;