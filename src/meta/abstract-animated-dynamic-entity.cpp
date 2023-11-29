#include <meta.hpp>

#include <algorithm>
#include <filesystem>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
AbstractAnimatedDynamicEntity<T>::~AbstractAnimatedDynamicEntity() {
    delete nextDestCoords;
    delete nextDestRect;
}

/**
 * @brief Call `move()` method on every instance of derived class `T`.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::moveAll() {
    for (auto& instance : instances) instance->move();
}

template <class T>
void AbstractAnimatedDynamicEntity<T>::onLevelChange(const level::EntityLevelData& entityLevelData) {
    AbstractEntity<T>::onLevelChange(entityLevelData);
    onMoveEnd();
}

/**
 * @brief Handle the movement of the entity from the current `Tile` to the next.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::move() {
    if (nextDestCoords == nullptr) return;   // DO NOT remove this - without this the program magically terminates itself.

    if (currMoveDelay == kMoveDelay) {
        destRect.x += currVelocity.x * kVelocity.x;
        destRect.y += currVelocity.y * kVelocity.y;

        // Continue movement if new `Tile` has not been reached
        if ((nextDestRect->x - destRect.x) * currVelocity.x > 0 || (nextDestRect->y - destRect.y) * currVelocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway

        // When new `Tile` has been reached, reset to IDLE state
        onMoveStart();
    }

    // Enable new movement based on `kMoveDelay`
    if (currMoveDelay) --currMoveDelay; else onMoveEnd();
}

/**
 * @brief Initiate the movement of the entity from the current `Tile` to the next.
 * @note Recommended implementation: this method should only be called after `currentVelocity` is guaranteed to change i.e. be assigned a non-zero value.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::initiateMove() {
    nextDestCoords = new SDL_Point({destCoords.x + currVelocity.x, destCoords.y + currVelocity.y});
    nextDestRect = new SDL_Rect(AbstractEntity<T>::getDestRectFromCoords(*nextDestCoords));

    if (validateMove()) onMoveStart(); else onMoveEnd(true);   // In case of invalidation, call `onMoveEnd()` with the `invalidated` flag set to `true`
}

/**
 * @brief Check whether moving the entity from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
 * @todo Check overlap between entities of different types.
*/
template <class T>
bool AbstractAnimatedDynamicEntity<T>::validateMove() {
    if ((currVelocity.x | currVelocity.y) == 0 || nextDestCoords == nullptr || nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::tileDestCount.x || nextDestCoords -> y >= globals::tileDestCount.y) return false;

    // Prevent `destCoords` overlap
    // Warning: expensive operation
    if (
        std::find_if(
            instances.begin(), instances.end(),
            [&](const auto& instance) {
                return (nextDestCoords->x == instance->destCoords.x && nextDestCoords->y == instance->destCoords.y);   // Not handled: `(instance->nextDestCoords != nullptr && nextDestCoords->x == instance->nextDestCoords->x && nextDestCoords->y == instance->nextDestCoords->y)`
            }
        ) != instances.end()
    ) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        static tile::TilelayerTilesetData* tilelayerTilesetData = nullptr;
        for (const auto& gid : globals::currentLevelData.tileCollection[coords.y][coords.x]) {
            tilelayerTilesetData = new tile::TilelayerTilesetData(utils::getTilesetData(globals::tilelayerTilesetDataCollection, gid));
            if (!gid && tilelayerTilesetData->properties["collision"] != "true") continue;
            return gid;
        }
        return 0;
    };

    int currCollisionLevel = findCollisionLevelGID(destCoords);
    int nextCollisionLevel = findCollisionLevelGID(*nextDestCoords);

    if (!nextCollisionLevel) return false;
    return currCollisionLevel == nextCollisionLevel;
}

/**
 * @note Called when a move is initiated after successful validation.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::onMoveStart() {
    if (currVelocity.x) flip = (currVelocity.x + 1) >> 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;   // The default direction of a sprite in a tileset is right
    AbstractAnimatedEntity<T>::resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kWalk);
}

/**
 * @note Called when a validated move is finalized, or when a move is invalidated.
 * @param invalidated only set to `true` when called after a move is invalidated.
*/
template <class T>
void AbstractAnimatedDynamicEntity<T>::onMoveEnd(bool invalidated) {
    // Terminate movement when reached new `Tile`
    if (nextDestCoords != nullptr && nextDestRect != nullptr && !invalidated) {
        destCoords = *nextDestCoords;
        destRect = *nextDestRect;
    }

    nextDestCoords = nullptr;
    nextDestRect = nullptr;
    currVelocity = {0, 0};

    currMoveDelay = kMoveDelay;

    AbstractAnimatedEntity<T>::resetAnimation(tile::AnimatedEntitiesTilesetData::AnimationType::kIdle);
}


template <class T>
const SDL_Point AbstractAnimatedDynamicEntity<T>::kVelocity = globals::config::kDefaultAnimatedDynamicEntityVelocity;


template class AbstractAnimatedDynamicEntity<Player>;
template class AbstractAnimatedDynamicEntity<Slime>;