#include <entities.hpp>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <type_traits>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractEntity<T>::AbstractEntity(SDL_Point const& destCoords) : mID(++sID_Counter), mDestCoords(destCoords), mDestRectModifier(config::entities::destRectModifier) {
    mSrcRect.w = sTilesetData->srcSize.x * sTilesetData->animationSize.x;
    mSrcRect.h = sTilesetData->srcSize.y * sTilesetData->animationSize.y;
}

/**
 * @see <utils.h> utils::loadTilesetsData
*/
template <typename T>
void AbstractEntity<T>::initialize() {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(kTilesetPath.c_str());
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    sTilesetData = new tile::EntitiesTilesetData;
    sTilesetData->initialize(document, globals::renderer);
}

template <typename T>
void AbstractEntity<T>::deinitialize() {
    if (sTilesetData != nullptr) {
        sTilesetData->deinitialize();
        sTilesetData = nullptr;
    }
    
    Multiton<T>::deinitialize();
    sID_Counter = 0;
}

/**
 * @brief Clear `instanceMapping` then call `onLevelChange()` method on every instance of derived class `T`.
 * @todo Allow only `level::EntityLevelData` and its subclasses. Try `<type_traits>` and `<concepts>`.
*/
template <typename T>
template <typename LevelData>
void AbstractEntity<T>::onLevelChangeAll(typename level::EntityLevelData::Collection<LevelData> const& entityLevelDataCollection) {
    Multiton<T>::deinitialize();
    sID_Counter = 0;

    for (const auto& entityLevelData : entityLevelDataCollection) {
        auto instance = instantiate(entityLevelData.destCoords);
        instance->onLevelChange(entityLevelData);
    }
}

/**
 * @brief Render the current sprite to the window.
 * @note Recommended implementation: this method requires `destRect` and `srcRect` to be set properly prior to being called.
*/
template <typename T>
void AbstractEntity<T>::render() const {
    SDL_RenderCopyEx(globals::renderer, sTilesetData->texture, &mSrcRect, &mDestRect, mAngle, pCenter, mFlip);
}

/**
 * @brief Update data members that depend on window dimensions.
*/
template <typename T>
void AbstractEntity<T>::onWindowChange() {
    mDestRect = getDestRectFromCoords(mDestCoords);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractEntity<T>::onLevelChange(level::EntityLevelData const& entityLevelData) {
    mSecondaryStats.initialize(mPrimaryStats);   // Prevent resetting secondary stats on player entity
    mDestCoords = entityLevelData.destCoords;
}

/**
 * Adjust `destRect` based on `tilesetData->animationSize` and `destRectModifier`.
 * @return A `SDL_Rect` representing the position of the instance of derived class `T`, relative to the window.
 * @see https://stackoverflow.com/questions/3127962/c-float-to-int
*/
template <typename T>
SDL_Rect AbstractEntity<T>::getDestRectFromCoords(SDL_Point const& coords) const {
    return {
        coords.x * globals::tileDestSize.x + globals::windowOffset.x
        + utils::castFloatToInt(mDestRectModifier.x * globals::tileDestSize.x)
        - (sTilesetData->animationSize.x - 1) / 2 * globals::tileDestSize.x
        - utils::castFloatToInt(globals::tileDestSize.x * sTilesetData->animationSize.x * (mDestRectModifier.w - 1) / 2),   // Apply `destRectModifier.x`, center `destRect` based on `tilesetData->animationSize.x` and `destRectModifier.w`
        coords.y * globals::tileDestSize.y + globals::windowOffset.y + utils::castFloatToInt(mDestRectModifier.y * globals::tileDestSize.y) - (sTilesetData->animationSize.y - 1) / 2 * globals::tileDestSize.y - utils::castFloatToInt(globals::tileDestSize.y * sTilesetData->animationSize.y * (mDestRectModifier.h - 1) / 2),   // Apply `destRectModifier.y`, center `destRect` based on `tilesetData->animationSize.y` and `destRectModifier.h`
        utils::castFloatToInt(globals::tileDestSize.x * sTilesetData->animationSize.x * mDestRectModifier.w),
        utils::castFloatToInt(globals::tileDestSize.y * sTilesetData->animationSize.y * mDestRectModifier.h),
    };
}

template <typename T>
std::size_t std::hash<AbstractEntity<T>>::operator()(AbstractEntity<T> const*& instance) const {
    return instance == nullptr ? std::hash<std::nullptr_t>{}(instance) : std::hash<SDL_Point>(instance->mDestCoords);
}

template <typename T>
bool std::equal_to<AbstractEntity<T>>::operator()(AbstractEntity<T> const*& first, AbstractEntity<T> const*& second) const {
    return (first == nullptr && second == nullptr) || (first && second && first->mDestCoords == second->mDestCoords);
}


template <typename T>
tile::EntitiesTilesetData* AbstractEntity<T>::sTilesetData = nullptr;

template <typename T>
int AbstractEntity<T>::sID_Counter = 0;


/**
 * @note Explicit Template Instantiation.
 * @note Bear fruition from 8 hours of debugging and not less than 50 articles from StackOverflow and other places.
 * @warning Recommended implementation: derived abstract classes MUST provide similar implementation at the end of the corresponding source files.
*/
template class AbstractEntity<PentacleProjectile>;
template class AbstractEntity<Player>;
template class AbstractEntity<Teleporter>;
template class AbstractEntity<Slime>;

template void AbstractEntity<Teleporter>::onLevelChangeAll<level::TeleporterLevelData>(const level::EntityLevelData::Collection<level::TeleporterLevelData>& entityLevelDataCollection);
template void AbstractEntity<Slime>::onLevelChangeAll<level::SlimeLevelData>(const level::EntityLevelData::Collection<level::SlimeLevelData>& entityLevelDataCollection);