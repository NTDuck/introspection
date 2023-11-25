#include <meta.hpp>

#include <filesystem>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
AbstractAnimatedDynamicEntity<T>::AbstractAnimatedDynamicEntity() : kVelocity(globals::config::kDefaultAnimatedDynamicEntityVelocity) {}

template <class T>
AbstractAnimatedDynamicEntity<T>::~AbstractAnimatedDynamicEntity() {
    delete nextDestCoords;
    delete nextDestRect;
}

template <class T>
void AbstractAnimatedDynamicEntity<T>::onLevelChange(const level::EntityLevelData& textureData) {
    AbstractEntity<T>::onLevelChange(textureData);
    onMoveEnd();
}

/**
 * @brief Handle the movement of the entity from the current `Tile` to the next.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::move() {
    if (nextDestCoords == nullptr) return;   // DO NOT remove this - without this the program magically terminates itself.

    isNextTileReached = false;

    destRect.x += currentVelocity.x * kVelocity.x;
    destRect.y += currentVelocity.y * kVelocity.y;

    // Continue movement until new `Tile` has been reached.
    if ((nextDestRect -> x - destRect.x) * currentVelocity.x > 0 || (nextDestRect -> y - destRect.y) * currentVelocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway

    // Terminate movement when reached new `Tile`
    isNextTileReached = true;

    destCoords = *nextDestCoords;
    destRect = *nextDestRect;
    onMoveEnd();
}

/**
 * @brief Check whether moving the entity from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
*/
template <class T>
bool AbstractAnimatedDynamicEntity<T>::validateMove() {
    if (nextDestCoords == nullptr || nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::tileDestCount.x || nextDestCoords -> y >= globals::tileDestCount.y) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        static tile::TilelayerTilesetData* tilelayerTilesetData = nullptr;
        for (const auto& gid : globals::currentLevelData.tileCollection[coords.y][coords.x]) {
            tilelayerTilesetData = new tile::TilelayerTilesetData(utils::getTilesetData(globals::tilelayerTilesetDataCollection, gid));
            if (!gid && tilelayerTilesetData->properties["collision"] != "true") continue;
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

/**
 * @note DO NOT assume this function's purpose based on its name.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::onMoveStart() {
    AbstractAnimatedEntity<T>::resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kWalk);
}

/**
 * @note DO NOT assume this function's purpose based on its name.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::onMoveEnd() {
    nextDestCoords = nullptr;
    nextDestRect = nullptr;
    currentVelocity = {0, 0};

    AbstractAnimatedEntity<T>::resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kIdle);
}


template class AbstractAnimatedDynamicEntity<Player>;