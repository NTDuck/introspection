#include <entities.hpp>

#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity(SDL_Point const& destCoords) : AbstractEntity<T>(destCoords) {
    resetAnimation(Animation::kIdle);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractAnimatedEntity<T>::onLevelChange(level::Data_Generic const& entityLevelData) {
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
        case Animation::kAttackMeele:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerAttack : Mixer::SFXName::kEntityAttack);
            break;

        case Animation::kDamaged:
            if (!isAnimationAtFirstSprite()) return;
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kEntityDamaged);
            break;

        case Animation::kDeath:
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
    
    if (mCurrAnimationUpdateCount >= static_cast<int>(sTilesetData.animationUpdateRate * sTilesetData[mCurrAnimationType].updateRateMultiplier)) {
        mCurrAnimationUpdateCount = 0;
        if (mCurrAnimationGID < sTilesetData[mCurrAnimationType].stopGID) {
            mCurrAnimationGID += sTilesetData.animationSize.x;
            if (mCurrAnimationGID / sTilesetData.srcCount.x != (mCurrAnimationGID - sTilesetData.animationSize.x) / sTilesetData.srcCount.x) mCurrAnimationGID += sTilesetData.srcCount.x * (sTilesetData.animationSize.y - 1);   // Originally intended for "flawed" tilesets where `sTilesetData.animationSize.x` > 'sTilesetData.srcCount.x`
        } else {
            // Deinitialize `nextAnimationData`
            if (pNextAnimationType != nullptr && mIsAnimationOnProgress) {
                delete pNextAnimationType;
                pNextAnimationType = nullptr;
                mIsAnimationOnProgress = false;
            }

            if (mCurrAnimationType == Animation::kDeath) return;   // The real permanent
            resetAnimation(sTilesetData[mCurrAnimationType].isPermanent ? Animation::kIdle : mCurrAnimationType);
        };
    }

    mSrcRect.x = mCurrAnimationGID % sTilesetData.srcCount.x * sTilesetData.srcSize.x;
    mSrcRect.y = mCurrAnimationGID / sTilesetData.srcCount.x * sTilesetData.srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <typename T>
void AbstractAnimatedEntity<T>::resetAnimation(Animation animationType, EntityStatus flag) {
    mCurrAnimationType = animationType;
    if (flag == EntityStatus::kContinued) return;
    mCurrAnimationGID = AbstractEntity<T>::sTilesetData[mCurrAnimationType].startGID;
}

/**
 * @brief Initiate a new animation based on `nextAnimationData`.
*/
template <typename T>
void AbstractAnimatedEntity<T>::initiateAnimation() {
    // Check for priority overlap
    if (mCurrAnimationType == Animation::kDeath) return;
    if (pNextAnimationType == nullptr || mIsAnimationOnProgress) return;

    resetAnimation(*pNextAnimationType);
    mIsAnimationOnProgress = true;
}


template class AbstractAnimatedEntity<PentacleProjectile>;

template class AbstractAnimatedEntity<Player>;

template class AbstractAnimatedEntity<Teleporter>;
template class AbstractAnimatedEntity<RedHandThroneTeleporter>;

template class AbstractAnimatedEntity<Slime>;

template class AbstractAnimatedEntity<OmoriLaptop>;
template class AbstractAnimatedEntity<OmoriLightBulb>;
template class AbstractAnimatedEntity<OmoriMewO>;