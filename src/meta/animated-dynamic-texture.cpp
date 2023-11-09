#include <filesystem>
#include <sstream>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>

AnimatedDynamicTextureWrapper::AnimatedDynamicTextureWrapper() {}

AnimatedDynamicTextureWrapper::~AnimatedDynamicTextureWrapper() {
    AnimatedTextureWrapper::~AnimatedTextureWrapper();
    delete nextDestCoords;
    delete nextDestRect;
}

/**
 * @brief Initialize the player and populate `rotatingGIDs` for srcRect rotation.
 * @see https://en.cppreference.com/w/cpp/utility/from_chars (hopefully better than `std::istringstream`)
*/
void AnimatedDynamicTextureWrapper::init_(std::filesystem::path xmlPath) {
    AnimatedTextureWrapper::init_(xmlPath);
    VELOCITY = config::VELOCITY_PLAYER;
    velocity = {0, 0};
    onMoveEnd();
}

void AnimatedDynamicTextureWrapper::blit() {
    BaseTextureWrapper::blit();
    onMoveEnd();
}

/**
 * @brief Handle ONLY moving the texture from the current `Tile` to the next. Validation is handled elsewhere.
*/
void AnimatedDynamicTextureWrapper::move() {
    if (nextDestCoords == nullptr) return;   // DO NOT remove this - without this the program magically terminates itself.

    destRect.x += velocity.x * VELOCITY.x;
    destRect.y += velocity.y * VELOCITY.y;

    // Continue movement until new `Tile` has been reached.
    if ((nextDestRect -> x - destRect.x) * velocity.x > 0 || (nextDestRect -> y - destRect.y) * velocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway

    // Terminate movement when reached new `Tile`
    destCoords = *nextDestCoords;
    destRect = *nextDestRect;
    onMoveEnd();
}

/**
 * @brief Check whether moving the texture from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
*/
bool AnimatedDynamicTextureWrapper::validateMove() {
    if (nextDestCoords == nullptr || nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::TILE_DEST_COUNT.x || nextDestCoords -> y >= globals::TILE_DEST_COUNT.y) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        static TilesetData* tilesetData = nullptr;
        for (const auto& gid : globals::currentLevel.tileCollection[coords.y][coords.x]) {
            tilesetData = new TilesetData(utils::getTilesetData(gid));
            if (!gid && tilesetData -> properties["collision"] != "true") continue;
            // if (!collisionTransitionLevel) collisionTransitionLevel = std::stoi(tilesetData -> properties["collision-transition"]) + tilesetData -> firstGID;
            return gid;
        }
        return 0;
    };

    int currCollisionLevel = findCollisionLevelGID(destCoords);
    int nextCollisionLevel = findCollisionLevelGID(*nextDestCoords);

    /**
     * @details The move is validated only if `nextCollisionLevel` exists, and one of the following scenarios occurs:
     * 1. `currCollisionLevel` is equal to `collisionTransitionLevel`
     * 2. `nextCollisionLevel` is equal to `collisionTransitionLevel`
     * 3. `currCollisionLevel` is equal to `nextCollisionLevel`
    */
    if (!nextCollisionLevel) return false;
    // return (currCollisionLevel == collisionTransitionLevel || nextCollisionLevel == collisionTransitionLevel || currCollisionLevel == nextCollisionLevel);
    return currCollisionLevel == nextCollisionLevel;
}

void AnimatedDynamicTextureWrapper::onMoveStart() {
    AnimatedTextureWrapper::resetAnimation("animation-walk");
}

void AnimatedDynamicTextureWrapper::onMoveEnd() {
    nextDestCoords = nullptr;
    nextDestRect = nullptr;
    velocity = {0, 0};

    AnimatedTextureWrapper::resetAnimation("animation-idle");
}