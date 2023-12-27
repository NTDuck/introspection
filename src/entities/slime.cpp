#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Slime::Slime(SDL_Point const& destCoords) : AbstractAnimatedDynamicEntity<Slime>(destCoords) {
    destRectModifier = globals::config::kDefaultSlimeDestRectModifier;
    kMoveInitiateRange = globals::config::kDefaultSlimeMoveInitiateRange;
    kAttackInitiateRange = globals::config::kDefaultSlimeAttackInitiateRange;
    kAttackRegisterRange = globals::config::kDefaultSlimeAttackRegisterRange;
    primaryStats = globals::config::kDefaultSlimePrimaryStats;
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
int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = globals::config::kDefaultSlimeMoveDelay;

template <>
SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = globals::config::kDefaultSlimeVelocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = globals::config::kTilesetPathSlime;