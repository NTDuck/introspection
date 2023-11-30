#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


Slime::Slime() {
    destRectModifier = globals::config::kDefaultSlimeDestRectModifier;
}

void Slime::calculateMoveAll(const SDL_Point& playerDestCoords) {
    for (auto& instance : instances) instance->calculateMove(playerDestCoords);
}

/**
 * @brief Calculate the movement of an instance based on the position of the player entity.
 * @note Use `<cstdlib>` instead of `<random>` for slight performance gains. (sacrifice crypt)
*/
void Slime::calculateMove(const SDL_Point& playerDestCoords) {
    if (nextDestCoords != nullptr) return;

    if (utils::generateRandomBinary()) {
        currVelocity.x += ((playerDestCoords.x > destCoords.x) << 1) - 1;
    } else {
        currVelocity.y += ((playerDestCoords.y > destCoords.y) << 1) - 1;
    }

    AbstractAnimatedDynamicEntity<Slime>::initiateMove();
}


template <>
const int AbstractAnimatedDynamicEntity<Slime>::kMoveDelay = globals::config::kDefaultSlimeMoveDelay;

template <>
const SDL_FPoint AbstractAnimatedDynamicEntity<Slime>::kVelocity = globals::config::kDefaultSlimeVelocity;

template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = globals::config::kTilesetPathSlime;