#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


Slime::Slime() {
    destRectModifier = globals::config::kDefaultSlimeDestRectModifier;
    destCoordsDetectRange = globals::config::kDefaultSlimeDestCoordsDetectRange;
}

/**
 * @brief Calculate the movement of an instance based on the position of the player entity.
 * @note Use `<cstdlib>` instead of `<random>` for slight performance gains. (sacrifice crypt)
*/
void Slime::calculateMove(const SDL_Point& playerDestCoords) {
    AbstractAnimatedDynamicEntity<Slime>::initiateMove();
    auto distance = utils::calculateDistance(destCoords, playerDestCoords);

    if (destCoordsDetectRange.x < distance && destCoordsDetectRange.y < distance) {
        nextVelocity = {0, 0};
        return;
    }

    nextVelocity = (utils::generateRandomBinary() ? SDL_Point{(playerDestCoords.x > destCoords.x) * 2 - 1, 0} : SDL_Point{0, (playerDestCoords.y > destCoords.y) * 2 - 1});
}

template <>
const int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = globals::config::kDefaultSlimeMoveDelay;

template <>
const SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = globals::config::kDefaultSlimeVelocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = globals::config::kTilesetPathSlime;