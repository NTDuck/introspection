#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


Slime::Slime() {
    destRectModifier = {0, -globals::tileDestSize.y, 5, 5};
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
        currentVelocity.x += ((playerDestCoords.x > destCoords.x) << 1) - 1;
    } else {
        currentVelocity.y += ((playerDestCoords.y > destCoords.y) << 1) - 1;
    }

    AbstractAnimatedDynamicEntity<Slime>::initiateMove();
}


template <>
const std::filesystem::path AbstractEntity<Slime>::kTilesetPath = globals::config::kTilesetPathSlime;