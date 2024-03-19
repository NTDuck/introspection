#include <entities.hpp>

#include <filesystem>
#include <type_traits>

#include <SDL.h>

#include <mixer.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractAnimatedEntity<T>::AbstractAnimatedEntity(SDL_Point const& destCoords) : AbstractEntity<T>(destCoords) {
    if (!sTilesetPath.empty()) resetAnimation(Animation::kIdle);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractAnimatedEntity<T>::onLevelChange(level::Data_Generic const& entityLevelData) {
    mBaseAnimation = Animation::kIdle;
    AbstractEntity<T>::onLevelChange(entityLevelData);
}

/**
 * @note `AnimationType::kWalk` & `AnimationType:kRun` are handled elsewhere.
 * @see https://stackoverflow.com/questions/41011900/equivalent-ternary-operator-for-constexpr-if
*/
template <typename T>
void AbstractAnimatedEntity<T>::handleSFX() const {
    // Separate for different `T`
    static constexpr int idleFrames = config::game::FPS >> 2;   // Magic number
    static int idleFramesTracker = idleFrames;

    if (idleFramesTracker) {
        --idleFramesTracker;
        return;
    }

    switch (mAnimation) {
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

        default: return;
    }
    
    idleFramesTracker = idleFrames;
}

/**
 * @brief Switch from one sprite to the next. Called every `animationUpdateRate` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <typename T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    ++mAnimationUpdateCount;

    if (mAnimationUpdateCount >= static_cast<int>(sTilesetData.animationUpdateRate * mAnimationData.updateRateMultiplier)) {
        mAnimationUpdateCount = 0;
        if (mAnimationGID < mAnimationData.stopGID) {
            mAnimationGID += sTilesetData.animationSize.x;
            if (mAnimationGID / sTilesetData.srcCount.x != (mAnimationGID - sTilesetData.animationSize.x) / sTilesetData.srcCount.x) mAnimationGID += sTilesetData.srcCount.x * (sTilesetData.animationSize.y - 1);   // Originally intended for "flawed" tilesets where `sTilesetData.animationSize.x` > 'sTilesetData.srcCount.x`
        } else {
            if (mAnimation == Animation::kDeath) return;   // The real permanent
            if (tile::Data_EntityTileset::getContinuity(mAnimation)) resetAnimation(mAnimation);
            else resetAnimation(mBaseAnimation, EntityStatus::kPrioritized);
        };
    }

    mSrcRect.x = mAnimationGID % sTilesetData.srcCount.x * sTilesetData.srcSize.x;
    mSrcRect.y = mAnimationGID / sTilesetData.srcCount.x * sTilesetData.srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <typename T>
void AbstractAnimatedEntity<T>::resetAnimation(Animation animation, EntityStatus flag) {
    if (flag != EntityStatus::kPrioritized && tile::Data_EntityTileset::getPriority(animation) < tile::Data_EntityTileset::getPriority(mAnimation)) return; 

    mAnimation = animation;
    mAnimationData = sTilesetData.at(mAnimation, sTilesetData.isMultiDirectional ? mDirection : tile::Data_EntityTileset::kDefaultDirection);
    if (flag != EntityStatus::kContinued) mAnimationGID = mAnimationData.startGID;
}


template class AbstractAnimatedEntity<Player>;

template class AbstractAnimatedEntity<OmoriLightBulb>;

template class AbstractAnimatedEntity<PlaceholderInteractable>;
template class AbstractAnimatedEntity<OmoriLaptop>;
template class AbstractAnimatedEntity<OmoriMewO>;
template class AbstractAnimatedEntity<OmoriCat_0>;
template class AbstractAnimatedEntity<OmoriCat_1>;
template class AbstractAnimatedEntity<OmoriCat_2>;
template class AbstractAnimatedEntity<OmoriCat_3>;
template class AbstractAnimatedEntity<OmoriCat_4>;
template class AbstractAnimatedEntity<OmoriCat_5>;
template class AbstractAnimatedEntity<OmoriCat_6>;
template class AbstractAnimatedEntity<OmoriCat_7>;

template class AbstractAnimatedEntity<PlaceholderTeleporter>;
template class AbstractAnimatedEntity<RedHandThrone>;

template class AbstractAnimatedEntity<Slime>;

template class AbstractAnimatedEntity<PentacleProjectile>;


DEFINE_ABSTRACT_ANIMATED_ENTITY(OmoriLightBulb, config::entities::omori_light_bulb)