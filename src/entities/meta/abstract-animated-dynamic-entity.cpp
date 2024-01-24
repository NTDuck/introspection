#include <entities.hpp>

#include <algorithm>
#include <filesystem>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Flow: move handling
 * ┌───────────────┐
 * │    Derived    │
 * └───┬───────────┘
 *     │
 *     │   ┌────────────────────┐
 *     └───┤ calculateMove(...) ├──────┐
 *         └───┬────────────────┘      │
 *             │                       │
 *             └───► nextVelocity      │
 *                                     │
 * ┌───────────────────────────────┐   │
 * │ AbstractAnimatedDynamicEntity │   │
 * └───┬───────────────────────────┘   │
 *     │                               │
 *     │   ┌────────────────┐          │
 *     ├───┤ initiateMove() ◄──────────┘
 *     │   └───┬───┬────────┘
 *     │       │   │
 *     │       │   └───► nextDestCoords, nextDestRect
 *     │       │
 *     │   ┌───▼────────────┐
 *     ├───┤ validateMove() │
 *     │   └───┬────────────┘
 *     │       │
 *     │       ├─────────────────┐
 *     │       │                 │
 *     │   ┌───▼───────────┐ ┌───▼─────────────────────────┐
 *     └───┤ onMoveStart() ├─┤ onMoveEnd(invalidated=true) │
 *         └───┬───────────┘ └───┬─────────────────────────┘
 *             │                 │
 *             └───► reset       └───► reset
 *                   to WALK           to IDLE
*/


template <typename T>
AbstractAnimatedDynamicEntity<T>::AbstractAnimatedDynamicEntity(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {}

template <typename T>
AbstractAnimatedDynamicEntity<T>::~AbstractAnimatedDynamicEntity() {
    if (nextDestCoords != nullptr) {
        delete nextDestCoords;
        nextDestCoords = nullptr;
    }
    if (nextDestRect != nullptr) {
        delete nextDestRect;
        nextDestRect = nullptr;
    }
    if (nextVelocity != nullptr) {
        delete nextVelocity;
        nextVelocity = nullptr;
    }
}

template <typename T>
void AbstractAnimatedDynamicEntity<T>::onWindowChange() {
    AbstractAnimatedEntity<T>::onWindowChange();
    calculateVelocityDependencies();
}

template <typename T>
void AbstractAnimatedDynamicEntity<T>::onLevelChange(level::EntityLevelData const& entityLevelData) {
    AbstractAnimatedEntity<T>::onLevelChange(entityLevelData);
    onMoveEnd();
}

/**
 * @brief Handle the movement of the entity from the current `Tile` to the next.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::move() {
    if (nextDestCoords == nullptr) return;   // Return if the move has not been "initiated"

    if (counterMoveDelay == kMoveDelay) {   // Only executed if 
        destRect.x += currVelocity.x * kIntegralVelocity.x;
        destRect.y += currVelocity.y * kIntegralVelocity.y;

        // Store the fractional part
        counterFractionalVelocity.x += kFractionalVelocity.x;
        counterFractionalVelocity.y += kFractionalVelocity.y;

        // Prevent movement loss by handling accumulative movement 
        if (counterFractionalVelocity.x >= 1) {
            destRect.x += currVelocity.x;
            --counterFractionalVelocity.x;
        }
        if (counterFractionalVelocity.y >= 1) {
            destRect.y += currVelocity.y;
            --counterFractionalVelocity.y;
        }

        // Continue movement if new `Tile` has not been reached
        if ((nextDestRect->x - destRect.x) * currVelocity.x > 0 || (nextDestRect->y - destRect.y) * currVelocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway
    }

    // Cease new movement if counter not done
    if (counterMoveDelay) { --counterMoveDelay; return; }

    // If new move has not been "initiated", terminate movement i.e. switch back to IDLE
    if (nextVelocity == nullptr) onMoveEnd();
    // If new move has been initiated, do it like this to avoid 
    else 

    // If new move 
    onMoveEnd(EntityStatusFlag::kContinued);
    initiateMove(EntityStatusFlag::kContinued);
}

/**
 * @brief Initiate the movement of the entity from the current `Tile` to the next.
 * @note Recommended implementation: this method should only be called after `currentVelocity` is guaranteed to change i.e. be assigned a non-zero value.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::initiateMove(EntityStatusFlag flag) {
    if (
        nextDestCoords != nullptr   // Another move is on progress
        || currAnimationType == AnimationType::kDeath   // Cannot move if is already dead
        || (currAnimationType == AnimationType::kDamaged || (nextAnimationType != nullptr && *nextAnimationType == AnimationType::kDamaged))   // Cannot move while damaged
    ) return;

    if (nextVelocity == nullptr) {
        onMoveEnd(EntityStatusFlag::kInvalidated);
        return;
    }

    nextDestCoords = new SDL_Point(destCoords + *nextVelocity);
    nextDestRect = new SDL_Rect(AbstractEntity<T>::getDestRectFromCoords(*nextDestCoords));

    if (validateMove()) onMoveStart(flag); else onMoveEnd(EntityStatusFlag::kInvalidated);   // In case of invalidation, call `onMoveEnd()` with the `invalidated` flag set to `true`
}

/**
 * @brief Check whether moving the entity from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
 * @todo Check overlap between entities of different types.
*/
template <typename T>
bool AbstractAnimatedDynamicEntity<T>::validateMove() const {
    if (nextDestCoords == nullptr || nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::tileDestCount.x || nextDestCoords -> y >= globals::tileDestCount.y) return false;

    // Prevent `destCoords` overlap
    // Warning: expensive operation
    if (
        std::find_if(
            instances.begin(), instances.end(),
            [&](const auto& instance) {
                return (*nextDestCoords == instance->destCoords);
            }
        ) != instances.end()
    ) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        for (const auto& gid : globals::currentLevelData.tileCollection[coords.y][coords.x]) {
            auto tilelayerTilesetData = utils::getTilesetData(globals::tilelayerTilesetDataCollection, gid);
            if (tilelayerTilesetData == nullptr) continue;

            auto it = tilesetData->properties.find("collision");
            if (!gid && it != tilesetData->properties.end() && it->second == "true") continue;

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
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveStart(EntityStatusFlag flag) {
    currVelocity = *nextVelocity;

    if (currVelocity.x) flip = (currVelocity.x + 1) >> 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;   // The default direction of a sprite in a tileset is right

    AbstractAnimatedEntity<T>::resetAnimation((!isRunning ? AnimationType::kWalk : AnimationType::kRunning), flag);
}

/**
 * @note Called when a validated move is finalized, or when a move is invalidated.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveEnd(EntityStatusFlag flag) {
    // Terminate movement when reached new `Tile`
    if (nextDestCoords != nullptr && nextDestRect != nullptr && flag != EntityStatusFlag::kInvalidated) {
        destCoords = *nextDestCoords;
        destRect = *nextDestRect;

        if (flag == EntityStatusFlag::kInvalidated) {
            delete nextDestCoords;
            delete nextDestRect;
        }
    }

    nextDestCoords = nullptr;
    nextDestRect = nullptr;

    currVelocity = {0, 0};
    counterMoveDelay = kMoveDelay;
    counterFractionalVelocity = {0, 0};

    if (flag == EntityStatusFlag::kContinued) return;
    AbstractAnimatedEntity<T>::resetAnimation(AnimationType::kIdle, flag);
}

/**
 * @brief Called when the entity starts or stops running. Change `kVelocity` and its dependencies.
 * @param onRunningStart governs whether this function is called to starts running or stops running.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onRunningToggled(bool onRunningStart) {
    if (!isRunning ^ onRunningStart) return;

    // Re-calculate certain dependencies
    kVelocity.x *= (onRunningStart ? 1 / runModifier : runModifier);
    kVelocity.y *= (onRunningStart ? 1 / runModifier : runModifier);
    calculateVelocityDependencies();

    // Switch to proper animation type
    if (currAnimationType == AnimationType::kWalk && onRunningStart) AbstractAnimatedEntity<T>::resetAnimation(AnimationType::kRunning);
    else if (currAnimationType == AnimationType::kRunning && !onRunningStart) AbstractAnimatedEntity<T>::resetAnimation(AnimationType::kWalk);

    // Don't forget to change this
    isRunning = onRunningStart;
}

/**
 * @brief Called should `kVelocity` changes.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::calculateVelocityDependencies() {
    // Each frame, for dimension `i`, the entity moves `globals::tileDestSize.i / kVelocity.i` (pixels), rounded down
    kIntegralVelocity = {
        utils::castFloatToInt(globals::tileDestSize.x / kVelocity.x),
        utils::castFloatToInt(globals::tileDestSize.y / kVelocity.y),
    };
    kFractionalVelocity = {
        globals::tileDestSize.x / kVelocity.x - kIntegralVelocity.x,
        globals::tileDestSize.y / kVelocity.y - kIntegralVelocity.y,
    }; 
}


template <typename T>
const double AbstractAnimatedDynamicEntity<T>::runModifier = config::entities::runVelocityModifier;


template class AbstractAnimatedDynamicEntity<Player>;
template class AbstractAnimatedDynamicEntity<Slime>;