#include <entities.hpp>

#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <type_traits>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
AbstractEntity<T>::AbstractEntity(SDL_Point const& destCoords) : destCoords(destCoords), destRectModifier(config::entities::destRectModifier) {
    srcRect.w = tilesetData->srcSize.x * tilesetData->animationSize.x;
    srcRect.h = tilesetData->srcSize.y * tilesetData->animationSize.y;
}

/**
 * @see <utils.h> utils::loadTilesetsData
*/
template <typename T>
void AbstractEntity<T>::initialize() {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(kTilesetPath.c_str());
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    tilesetData = new tile::EntitiesTilesetData;
    tilesetData->initialize(document, globals::renderer);
}

template <typename T>
void AbstractEntity<T>::deinitialize() {
    Multiton<T>::deinitialize();
    if (tilesetData != nullptr) {
        tilesetData->deinitialize();
        tilesetData = nullptr;
    }
}

/**
 * @brief Clear `instanceMapping` then call `onLevelChange()` method on every instance of derived class `T`.
 * @todo Allow only `level::EntityLevelData` and its subclasses. Try `<type_traits>` and `<concepts>`.
*/
template <typename T>
template <typename LevelData>
void AbstractEntity<T>::onLevelChangeAll(typename level::EntityLevelData::Collection<LevelData> const& entityLevelDataCollection) {
    Multiton<T>::deinitialize();

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
    SDL_RenderCopyEx(globals::renderer, tilesetData->texture, &srcRect, &destRect, angle, center, flip);
}

/**
 * @brief Update data members that depend on window dimensions.
*/
template <typename T>
void AbstractEntity<T>::onWindowChange() {
    destRect = getDestRectFromCoords(destCoords);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <typename T>
void AbstractEntity<T>::onLevelChange(level::EntityLevelData const& entityLevelData) {
    secondaryStats.initialize(primaryStats);   // Prevent resetting secondary stats on player entity
    destCoords = entityLevelData.destCoords;
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
        + utils::castFloatToInt(destRectModifier.x * globals::tileDestSize.x)
        - (tilesetData->animationSize.x - 1) / 2 * globals::tileDestSize.x
        - utils::castFloatToInt(globals::tileDestSize.x * tilesetData->animationSize.x * (destRectModifier.w - 1) / 2),   // Apply `destRectModifier.x`, center `destRect` based on `tilesetData->animationSize.x` and `destRectModifier.w`
        coords.y * globals::tileDestSize.y + globals::windowOffset.y + utils::castFloatToInt(destRectModifier.y * globals::tileDestSize.y) - (tilesetData->animationSize.y - 1) / 2 * globals::tileDestSize.y - utils::castFloatToInt(globals::tileDestSize.y * tilesetData->animationSize.y * (destRectModifier.h - 1) / 2),   // Apply `destRectModifier.y`, center `destRect` based on `tilesetData->animationSize.y` and `destRectModifier.h`
        utils::castFloatToInt(globals::tileDestSize.x * tilesetData->animationSize.x * destRectModifier.w),
        utils::castFloatToInt(globals::tileDestSize.y * tilesetData->animationSize.y * destRectModifier.h),
    };
}

template <typename T>
std::size_t std::hash<AbstractEntity<T>>::operator()(AbstractEntity<T> const*& instance) const {
    return instance == nullptr ? std::hash<std::nullptr_t>{}(instance) : std::hash<SDL_Point>(instance->destCoords);
}

template <typename T>
bool std::equal_to<AbstractEntity<T>>::operator()(AbstractEntity<T> const*& first, AbstractEntity<T> const*& second) const {
    return (first == nullptr && second == nullptr) || (first && second && first->destCoords == second->destCoords);
}


template <typename T>
tile::EntitiesTilesetData* AbstractEntity<T>::tilesetData = nullptr;


/**
 * @note Explicit Template Instantiation.
 * @note Bear fruition from 8 hours of debugging and not less than 50 articles from StackOverflow and other places.
 * @warning Recommended implementation: derived abstract classes MUST provide similar implementation at the end of the corresponding source files.
*/
template class AbstractEntity<Player>;
template class AbstractEntity<Teleporter>;
template class AbstractEntity<Slime>;

template void AbstractEntity<Teleporter>::onLevelChangeAll<level::TeleporterLevelData>(const level::EntityLevelData::Collection<level::TeleporterLevelData>& entityLevelDataCollection);
template void AbstractEntity<Slime>::onLevelChangeAll<level::SlimeLevelData>(const level::EntityLevelData::Collection<level::SlimeLevelData>& entityLevelDataCollection);