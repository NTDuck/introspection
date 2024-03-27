#include <entities.hpp>

#include <algorithm>
#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractAnimatedDynamicEntity<T>::AbstractAnimatedDynamicEntity(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords), mMoveDelayTimer(sMoveDelayTicks) {
    onMoveEnd();
}

template <typename T>
AbstractAnimatedDynamicEntity<T>::~AbstractAnimatedDynamicEntity() {
    if (mNextDestCoords != nullptr) {
        delete mNextDestCoords;
        mNextDestCoords = nullptr;
    }
    if (mNextDestRect != nullptr) {
        delete mNextDestRect;
        mNextDestRect = nullptr;
    }
    if (mNextVelocity != nullptr) {
        delete mNextVelocity;
        mNextVelocity = nullptr;
    }
}

template <typename T>
void AbstractAnimatedDynamicEntity<T>::onWindowChange() {
    AbstractAnimatedEntity<T>::onWindowChange();
    calculateVelocityDependencies();
}

template <typename T>
bool AbstractAnimatedDynamicEntity<T>::isWithinRange(std::pair<int, int> const& x_coords_lim, std::pair<int, int> const& y_coords_lim) const {
    return isTargetWithinRange(mNextDestCoords != nullptr ? *mNextDestCoords : mDestCoords, x_coords_lim, y_coords_lim);
}

/**
 * @brief Handle the movement of the entity from the current `Tile` to the next.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::move() {
    if (mNextDestCoords == nullptr || !mMoveDelayTimer.isFinished()) return;   // Return if the move has not been "initiated"

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
    if ((mNextDestRect->x - mDestRect.x) * mCurrVelocity.x > 0 || (mNextDestRect->y - mDestRect.y) * mCurrVelocity.y > 0) return;   // Not sure this is logically acceptable but this took 3 hours of debugging so just gonna keep it anyway

    if (mNextVelocity == nullptr) onMoveEnd();   // If new move has not been initiated, terminate movement i.e. switch back to `mBaseAnimation`
    else {
        onMoveEnd(BehaviouralType::kContinued);
        initiateMove(BehaviouralType::kContinued);
    }
}

/**
 * @brief Initiate the movement of the entity from the current `Tile` to the next.
 * @note Recommended implementation: this method should only be called after `currentVelocity` is guaranteed to change i.e. be assigned a non-zero value.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::initiateMove(BehaviouralType flag) {
    if ( (mNextDestCoords != nullptr && mNextVelocity != nullptr && *mNextVelocity + mCurrVelocity != SDL_Point{ 0, 0 }) || mAnimation == Animation::kDeath) return;   // Another move is on progress and next move is not directionally opposite to current move

    if (mNextVelocity == nullptr) { onMoveEnd(BehaviouralType::kInvalidated); return; }

    if (sTilesetData.isMultiDirectional) mPrevDirection = mDirection;
    mDirection = *mNextVelocity;
    if (!sTilesetData.isMultiDirectional && mDirection.x) mFlip = (mDirection.x + 1) >> 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;   // The default direction of a sprite in a tileset is right

    mNextDestCoords = new SDL_Point(mDestCoords + mDirection);
    mNextDestRect = new SDL_Rect(AbstractEntity<T>::getDestRectFromCoords(*mNextDestCoords));

    if (validateMove()) onMoveStart(flag); else onMoveEnd(BehaviouralType::kInvalidated);   // In case of invalidation, call `onMoveEnd()` with the `invalidated` flag set to `true`
}

/**
 * @brief Check whether moving the entity from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
 * @todo Check overlap between entities of different types.
*/
template <typename T>
bool AbstractAnimatedDynamicEntity<T>::validateMove() const {
    if (mNextDestCoords == nullptr || mNextDestCoords -> x < 0 || mNextDestCoords -> y < 0 || mNextDestCoords -> x >= level::data.tileDestCount.x || mNextDestCoords -> y >= level::data.tileDestCount.y) return false;

    // Prevent `destCoords` overlap
    // Warning: expensive operation
    if constexpr(!config::enable_entity_overlap) if (std::find_if( instances.begin(), instances.end(), [&](const auto& instance) { return (*mNextDestCoords == instance->mDestCoords); }) != instances.end()) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](SDL_Point const& coords) {
        return level::data.collisionTilelayer[coords.y][coords.x];
    };

    int currCollisionLevel = findCollisionLevelGID(mDestCoords);
    int nextCollisionLevel = findCollisionLevelGID(*mNextDestCoords);

    if (!nextCollisionLevel) return false;
    return currCollisionLevel == nextCollisionLevel;
}

/**
 * @note Called when a move is initiated after successful validation.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveStart(BehaviouralType flag) {
    mCurrVelocity = *mNextVelocity;
    mBaseAnimation = mIsRunning ? Animation::kRun : Animation::kWalk;

    if (sTilesetData.isMultiDirectional && mDirection != mPrevDirection) flag = BehaviouralType::kDefault;   // Only switch to `startGID` if tileset is multidirectional and direction has recently changed

    resetAnimation(mBaseAnimation, flag);
}

/**
 * @note Called when a validated move is finalized, or when a move is invalidated.
*/
template <typename T>
void AbstractAnimatedDynamicEntity<T>::onMoveEnd(BehaviouralType flag) {
    // Terminate movement when reached new `Tile`
    if (mNextDestCoords != nullptr && mNextDestRect != nullptr) {
        if (flag != BehaviouralType::kInvalidated) {
            mDestCoords = *mNextDestCoords;
            mDestRect = *mNextDestRect;
        }

        delete mNextDestCoords;
        mNextDestCoords = nullptr;

        delete mNextDestRect;
        mNextDestRect = nullptr;
    }

    mMoveDelayTimer.start();
    mCurrVelocity = { 0, 0 };
    mFractionalVelocityCounter = { 0, 0 };

    mBaseAnimation = Animation::kIdle;
    resetAnimation(mBaseAnimation, flag);
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
    mBaseAnimation = onRunningStart ? Animation::kRun : Animation::kWalk;
    if (mNextDestCoords != nullptr) resetAnimation(mBaseAnimation);

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
        utils::ftoi(level::data.tileDestSize.x / sVelocity.x),
        utils::ftoi(level::data.tileDestSize.y / sVelocity.y),
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