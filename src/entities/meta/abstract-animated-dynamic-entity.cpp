#include <entities.hpp>

#include <algorithm>
#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
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
    if (pNextDestCoords != nullptr) {
        delete pNextDestCoords;
        pNextDestCoords = nullptr;
    }
    if (pNextDestRect != nullptr) {
        delete pNextDestRect;
        pNextDestRect = nullptr;
    }
    if (pNextVelocity != nullptr) {
        delete pNextVelocity;
        pNextVelocity = nullptr;
    }
}

template <typename T>
void AbstractAnimatedDynamicEntity<T>::onWindowChange() {
    AbstractAnimatedEntity<T>::onWindowChange();
    calculateVelocityDependencies();
}

template <typename T>
void AbstractAnimatedDynamicEntity<T>::onLevelChange(level::Data_Generic const& entityLevelData) {
    AbstractAnimatedEntity<T>::onLevelChange(entityLevelData);
    onMoveEnd();
}

template <typename T>
bool AbstractAnimatedDynamicEntity<T>::isWithinRange(std::pair<int, int> const& x_coords_lim, std::pair<int, int> const& y_coords_lim) const {
    return isTargetWithinRange(pNextDestCoords != nullptr ? *pNextDestCoords : mDestCoords, x_coords_lim, y_coords_lim);
}

/**
 * @brief Handle the movement of the entity from the current `Tile` to the next.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::move() {
    if (pNextDestCoords == nullptr) return;   // Return if the move has not been "initiated"

    if (mMoveDelayCounter == sMoveDelay) {   // Only executed if 
        mDestRect.x += mCurrVelocity.x * mIntegralVelocity.x;
        mDestRect.y += mCurrVelocity.y * mIntegralVelocity.y;

        // Store the fractional part
        mFractionalVelocityCounter.x += mFractionalVelocity.x;
        mFractionalVelocityCounter.y += mFractionalVelocity.y;

        // Prevent movement loss by handling accumulative movement 
        if (mFractionalVelocityCounter.x >= 1) {
            mDestRect.x += mCurrVelocity.x;
            --mFractionalVelocityCounter.x;
        }
        if (mFractionalVelocityCounter.y >= 1) {
            mDestRect.y += mCurrVelocity.y;
            --mFractionalVelocityCounter.y;
        }

        // Continue movement if new `Tile` has not been reached
        if ((pNextDestRect->x - mDestRect.x) * mCurrVelocity.x > 0 || (pNextDestRect->y - mDestRect.y) * mCurrVelocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway
    }

    // Cease new movement if counter not done
    if (mMoveDelayCounter) { --mMoveDelayCounter; return; }

    // If new move has not been "initiated", terminate movement i.e. switch back to IDLE
    if (pNextVelocity == nullptr) onMoveEnd();
    // If new move has been initiated, do this to avoid switching back to original GID
    else {
        onMoveEnd(EntityStatus::kContinued);
        initiateMove(EntityStatus::kContinued);
    }
}

/**
 * @brief Initiate the movement of the entity from the current `Tile` to the next.
 * @note Recommended implementation: this method should only be called after `currentVelocity` is guaranteed to change i.e. be assigned a non-zero value.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::initiateMove(EntityStatus flag) {
    if (
        pNextDestCoords != nullptr   // Another move is on progress
        || mCurrAnimationType == Animation::kDeath   // Cannot move if is already dead
        // || (currAnimationType == AnimationType::kDamaged || (nextAnimationType != nullptr && *nextAnimationType == AnimationType::kDamaged))   // Cannot move while damaged
    ) return;

    if (pNextVelocity == nullptr) { onMoveEnd(EntityStatus::kInvalidated); return; }

    if (pNextVelocity->x) mFlip = (pNextVelocity->x + 1) >> 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;   // The default direction of a sprite in a tileset is right

    pNextDestCoords = new SDL_Point(mDestCoords + *pNextVelocity);
    pNextDestRect = new SDL_Rect(AbstractEntity<T>::getDestRectFromCoords(*pNextDestCoords));

    if (validateMove()) onMoveStart(flag); else onMoveEnd(EntityStatus::kInvalidated);   // In case of invalidation, call `onMoveEnd()` with the `invalidated` flag set to `true`
}

/**
 * @brief Check whether moving the entity from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
 * @todo Check overlap between entities of different types.
*/
template <typename T>
bool AbstractAnimatedDynamicEntity<T>::validateMove() const {
    if (pNextDestCoords == nullptr || pNextDestCoords -> x < 0 || pNextDestCoords -> y < 0 || pNextDestCoords -> x >= level::data.tileDestCount.x || pNextDestCoords -> y >= level::data.tileDestCount.y) return false;

    // Prevent `destCoords` overlap
    // Warning: expensive operation
    if (
        std::find_if(
            instances.begin(), instances.end(),
            [&](const auto& instance) {
                return (*pNextDestCoords == instance->mDestCoords);
            }
        ) != instances.end()
    ) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        return level::data.collisionTilelayer[coords.y][coords.x];
    };

    int currCollisionLevel = findCollisionLevelGID(mDestCoords);
    int nextCollisionLevel = findCollisionLevelGID(*pNextDestCoords);

    if (!nextCollisionLevel) return false;
    return currCollisionLevel == nextCollisionLevel;
}

/**
 * @note Called when a move is initiated after successful validation.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveStart(EntityStatus flag) {
    mCurrVelocity = *pNextVelocity;

    AbstractAnimatedEntity<T>::resetAnimation((mIsRunning ? Animation::kRun : Animation::kWalk), flag);
}

/**
 * @note Called when a validated move is finalized, or when a move is invalidated.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveEnd(EntityStatus flag) {
    // Terminate movement when reached new `Tile`
    if (pNextDestCoords != nullptr && pNextDestRect != nullptr && flag != EntityStatus::kInvalidated) {
        mDestCoords = *pNextDestCoords;
        mDestRect = *pNextDestRect;

        if (flag == EntityStatus::kInvalidated) {
            delete pNextDestCoords;
            delete pNextDestRect;
        }
    }

    pNextDestCoords = nullptr;
    pNextDestRect = nullptr;

    mCurrVelocity = {0, 0};
    mMoveDelayCounter = sMoveDelay;
    mFractionalVelocityCounter = {0, 0};

    if (flag == EntityStatus::kContinued) return;
    AbstractAnimatedEntity<T>::resetAnimation(Animation::kIdle, flag);
}

/**
 * @brief Called when the entity starts or stops running. Change `kVelocity` and its dependencies.
 * @param onRunningStart governs whether this function is called to starts running or stops running.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onRunningToggled(bool onRunningStart) {
    if (!mIsRunning ^ onRunningStart) return;

    // Re-calculate certain dependencies
    sVelocity.x *= (onRunningStart ? 1 / sRunModifier : sRunModifier);
    sVelocity.y *= (onRunningStart ? 1 / sRunModifier : sRunModifier);
    calculateVelocityDependencies();

    // Switch to proper animation type
    if (mCurrAnimationType == Animation::kWalk && onRunningStart) AbstractAnimatedEntity<T>::resetAnimation(Animation::kRun);
    else if (mCurrAnimationType == Animation::kRun && !onRunningStart) AbstractAnimatedEntity<T>::resetAnimation(Animation::kWalk);

    // Don't forget to change this
    mIsRunning = onRunningStart;
}

/**
 * @brief Called should `kVelocity` changes.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::calculateVelocityDependencies() {
    // Each frame, for dimension `i`, the entity moves `level::data.tileDestSize.i / kVelocity.i` (pixels), rounded down
    mIntegralVelocity = {
        utils::castFloatToInt(level::data.tileDestSize.x / sVelocity.x),
        utils::castFloatToInt(level::data.tileDestSize.y / sVelocity.y),
    };
    mFractionalVelocity = {
        level::data.tileDestSize.x / sVelocity.x - mIntegralVelocity.x,
        level::data.tileDestSize.y / sVelocity.y - mIntegralVelocity.y,
    };
}


template <typename T>
const double AbstractAnimatedDynamicEntity<T>::sRunModifier = config::entities::runVelocityModifier;


template class AbstractAnimatedDynamicEntity<Player>;

template class AbstractAnimatedDynamicEntity<Slime>;

template class AbstractAnimatedDynamicEntity<PentacleProjectile>;