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

template <typename T>
void AbstractAnimatedEntity<T>::reinitialize(std::filesystem::path path) {
    AbstractEntity<T>::reinitialize(path);
    invoke(&T::resetAnimation, Animation::kIdle, BehaviouralType::kDefault);
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
    static CountdownTimer timer(config::entities::SFXTicks);
    if (!timer.isStarted()) timer.start();
    if (!timer.isFinished()) return;

    switch (mAnimation) {
        case Animation::kAttackMeele:
            if (!isAnimationAtFirstSprite()) break;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerAttack : Mixer::SFXName::kEntityAttack);
            break;

        case Animation::kDamaged:
            if (!isAnimationAtFirstSprite()) break;
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kEntityDamaged);
            break;

        case Animation::kDeath:
            if (!isAnimationAtFirstSprite()) break;
            Mixer::invoke(&Mixer::playSFX, std::is_same_v<T, Player> ? Mixer::SFXName::kPlayerDeath : Mixer::SFXName::kEntityDeath);
            break;

        default: break;
    }

    timer.start();
}

/**
 * @brief Switch from one sprite to the next. Called every `animationTicks` frames.
 * @see <interface.h> Interface::renderLevelTiles()
*/
template <typename T>
void AbstractAnimatedEntity<T>::updateAnimation() {
    if (!mAnimationTimer.isStarted()) mAnimationTimer.start();
    if (mAnimationTimer.isFinished()) {
        mAnimationTimer.stop();

        if (mAnimationGID < mAnimationData.stopGID) {
            mAnimationGID += sTilesetData.animationSize.x;
            if (mAnimationGID / sTilesetData.srcCount.x != (mAnimationGID - sTilesetData.animationSize.x) / sTilesetData.srcCount.x) mAnimationGID += sTilesetData.srcCount.x * (sTilesetData.animationSize.y - 1);   // Originally intended for "flawed" tilesets where `sTilesetData.animationSize.x` > 'sTilesetData.srcCount.x`
        } else {
            if (mAnimation == Animation::kDeath) return;   // The real permanent
            if (tile::Data_EntityTileset::getContinuity(mAnimation)) resetAnimation(mAnimation);
            else resetAnimation(mBaseAnimation, BehaviouralType::kPrioritized);
        }
    }

    mSrcRect.x = mAnimationGID % sTilesetData.srcCount.x * sTilesetData.srcSize.x;
    mSrcRect.y = mAnimationGID / sTilesetData.srcCount.x * sTilesetData.srcSize.y;
}

/**
 * @brief Switch to new animation type i.e. new collection of sprites.
*/
template <typename T>
void AbstractAnimatedEntity<T>::resetAnimation(Animation animation, BehaviouralType flag) {
    if (flag != BehaviouralType::kPrioritized && tile::Data_EntityTileset::getPriority(animation) < tile::Data_EntityTileset::getPriority(mAnimation)) return; 

    mAnimation = animation;
    mAnimationData = sTilesetData.at(mAnimation, sTilesetData.isMultiDirectional ? mDirection : tile::Data_EntityTileset::kDefaultDirection);

    mAnimationTimer.setMaxTicks(sTilesetData.animationTicks * mAnimationData.ticksMultiplier);

    if (flag != BehaviouralType::kContinued) mAnimationGID = mAnimationData.startGID;
}


template class AbstractAnimatedEntity<Player>;

template class AbstractAnimatedEntity<OmoriLightBulb>;
template class AbstractAnimatedEntity<OmoriKeysWASD>;
template class AbstractAnimatedEntity<HospitalXRayMachine>;

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
template class AbstractAnimatedEntity<HospitalXRayScreenArm>;
template class AbstractAnimatedEntity<HospitalXRayScreenHead>;
template class AbstractAnimatedEntity<HospitalSink>;

template class AbstractAnimatedEntity<PlaceholderTeleporter>;
template class AbstractAnimatedEntity<RedHandThrone>;

template class AbstractAnimatedEntity<Slime>;
template class AbstractAnimatedEntity<PixelCatGray>;
template class AbstractAnimatedEntity<PixelCatGold>;

template class AbstractAnimatedEntity<PentacleProjectile>;


DEF_ABSTRACT_ANIMATED_ENTITY(OmoriLightBulb, config::entities::omori_light_bulb)
DEF_ABSTRACT_ANIMATED_ENTITY(OmoriKeysWASD, config::entities::omori_keys_wasd)
DEF_ABSTRACT_ANIMATED_ENTITY(HospitalXRayMachine, config::entities::hospital_xray_machine)