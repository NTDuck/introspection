#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Slime::Slime(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<Slime>(destCoords) {
    destRectModifier = config::slime::destRectModifier;
    kMoveInitiateRange = config::slime::moveInitiateRange;
    kAttackInitiateRange = config::slime::attackInitiateRange;
    kAttackRegisterRange = config::slime::attackRegisterRange;
    primaryStats = config::slime::primaryStats;
}

/**
 * @brief Calculate the movement of an instance based on the position of the player entity.
 * @note Use `<cstdlib>` instead of `<random>` for slight performance gains. (sacrifice crypt)
*/
void Slime::calculateMove(SDL_Point const& playerDestCoords) {
    auto distance = utils::calculateDistance(destCoords, playerDestCoords);
    if (kMoveInitiateRange.x < distance && kMoveInitiateRange.y < distance) {
        nextVelocity = {0, 0};
        return;
    }

    nextVelocity = (utils::generateRandomBinary() ? SDL_Point{(playerDestCoords.x > destCoords.x) * 2 - 1, 0} : SDL_Point{0, (playerDestCoords.y > destCoords.y) * 2 - 1});
    AbstractAnimatedDynamicEntity<Slime>::initiateMove();
}

template <>
int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = config::slime::moveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = config::slime::velocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = config::slime::path;