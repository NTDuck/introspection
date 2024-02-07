#include <entities.hpp>

#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity(SDL_Point const& destCoords) : AbstractEntity<T>(destCoords) {
    resetAnimation(AnimationType::kIdle);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractAnimatedEntity<T>::onLevelChange(level::EntityLevelData const& entityLevelData) {
    if (pNextAnimationType != nullptr) {
        delete pNextAnimationType;
        pNextAnimationType = nullptr;
        mIsAnimationOnProgress = false;
    }
    
    AbstractEntity<T>::onLevelChange(entityLevelData);
}

/**
 * @note `AnimationType::kWalk` & `AnimationType:kRun` are handled elsewhere.
 * @see https://stackoverflow.com/questions/41011900/equivalent-ternary-operator-for-constexpr-if
*/
template <typename T>
void AbstractAnimatedEntity<T>::handleSFX() const {
    switch (mCurrAnimationType) {
        case AnimationType::kAttack:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerAttack : Mixer::SFXName::kEntityAttack);
            break;

        case AnimationType::kDamaged:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kEntityDamaged);
            break;

        case AnimationType::kDeath:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerDeath : Mixer::SFXName::kEntityDeath);
            break;

        default: break;
    }
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <typename T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    ++mCurrAnimationUpdateCount;
    
    if (mCurrAnimationUpdateCount >= static_cast<int>(sTilesetData->animationUpdateRate * sTilesetData->animationMapping[mCurrAnimationType].animationUpdateRateMultiplier)) {
        mCurrAnimationUpdateCount = 0;
        if (mCurrAnimationGID < sTilesetData->animationMapping[mCurrAnimationType].stopGID) {
            mCurrAnimationGID += sTilesetData->animationSize.x;
            if (mCurrAnimationGID / sTilesetData->srcCount.x != (mCurrAnimationGID - sTilesetData->srcCount.x) / sTilesetData->srcCount.x) mCurrAnimationGID += sTilesetData->srcCount.x * (sTilesetData->animationSize.y - 1);   // Behold, heresy!
        } else {
            // Deinitialize `nextAnimationData`
            if (pNextAnimationType != nullptr && mIsAnimationOnProgress) {
                delete pNextAnimationType;
                pNextAnimationType = nullptr;
                mIsAnimationOnProgress = false;
            }

            if (mCurrAnimationType == AnimationType::kDeath) return;   // The real permanent
            resetAnimation(sTilesetData->animationMapping[mCurrAnimationType].isPermanent ? AnimationType::kIdle : mCurrAnimationType);
        };
    }

    mSrcRect.x = mCurrAnimationGID % sTilesetData->srcCount.x * sTilesetData->srcSize.x;
    mSrcRect.y = mCurrAnimationGID / sTilesetData->srcCount.x * sTilesetData->srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <typename T>
void AbstractAnimatedEntity<T>::resetAnimation(AnimationType animationType, EntityStatusFlag flag) {
    mCurrAnimationType = animationType;
    if (flag == EntityStatusFlag::kContinued) return;
    mCurrAnimationGID = AbstractEntity<T>::sTilesetData->animationMapping[mCurrAnimationType].startGID;
}

/**
 * @brief Initiate a new animation based on `nextAnimationData`.
*/
template <typename T>
void AbstractAnimatedEntity<T>::initiateAnimation() {
    // Check for priority overlap
    if (mCurrAnimationType == AnimationType::kDeath) return;
    if (pNextAnimationType == nullptr || mIsAnimationOnProgress) return;

    resetAnimation(*pNextAnimationType);
    mIsAnimationOnProgress = true;
}


template class AbstractAnimatedEntity<PentacleProjectile>;
template class AbstractAnimatedEntity<Player>;
template class AbstractAnimatedEntity<Teleporter>;
template class AbstractAnimatedEntity<Slime>;