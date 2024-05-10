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
    mSrcRect.w = sTilesetData.srcSize.x * sTilesetData.animationSize.x;
    mSrcRect.h = sTilesetData.srcSize.y * sTilesetData.animationSize.y;
}

/**
 * @see <utils.h> utils::loadTilesetsData
*/
template <typename T>
void AbstractEntity<T>::initialize() {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(sTilesetPath.c_str()); if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    sTilesetData.load(document, globals::renderer);
}

template <typename T>
void AbstractEntity<T>::deinitialize() {
    sTilesetData.clear();
    Multiton<T>::deinitialize();
    sID_Counter = 0;
}

/**
 * @brief Change `sTilesetData`.
 * @note Experimental - only intended to use for `Player`, since undefined behaviour would occur on existing entities (that use old `sTilesetData`).
*/
template <typename T>
void AbstractEntity<T>::reinitialize(std::filesystem::path path) {
    sTilesetData.clear();
    sTilesetPath = path;
    AbstractEntity<T>::initialize();
}


/**
 * @brief Render the current sprite to the window.
 * @note Recommended implementation: this method requires `destRect` and `srcRect` to be set properly prior to being called.
*/
template <typename T>
void AbstractEntity<T>::render() const {
    SDL_RenderCopyEx(globals::renderer, sTilesetData.texture, &mSrcRect, &mDestRect, mAngle, mCenter, mFlip);
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
void AbstractEntity<T>::onLevelChange(level::Data_Generic const& entityLevelData) {
    mDestCoords = entityLevelData.destCoords;
    mAttributes.heal();
}

template <typename T>
bool AbstractEntity<T>::isWithinRange(std::pair<int, int> const& x_coords_lim, std::pair<int, int> const& y_coords_lim) const {
    return isTargetWithinRange(mDestCoords, x_coords_lim, y_coords_lim);
}

/**
 * Adjust `destRect` based on `tilesetData->animationSize` and `destRectModifier`.
 * @return A `SDL_Rect` representing the position of the instance of derived class `T`, relative to the window.
 * @see https://stackoverflow.com/questions/3127962/c-float-to-int
*/
template <typename T>
SDL_Rect AbstractEntity<T>::getDestRectFromCoords(SDL_Point const& coords) const {
    return {
        coords.x * level::data.tileDestSize.x + utils::ftoi(mDestRectModifier.x * level::data.tileDestSize.x)
        - (sTilesetData.animationSize.x - 1) / 2 * level::data.tileDestSize.x
        - utils::ftoi(level::data.tileDestSize.x * sTilesetData.animationSize.x * (mDestRectModifier.w - 1) / 2),   // Apply `destRectModifier.x`, center `destRect` based on `tilesetData->animationSize.x` and `destRectModifier.w`
        coords.y * level::data.tileDestSize.y + utils::ftoi(mDestRectModifier.y * level::data.tileDestSize.y) - (sTilesetData.animationSize.y - 1) / 2 * level::data.tileDestSize.y - utils::ftoi(level::data.tileDestSize.y * sTilesetData.animationSize.y * (mDestRectModifier.h - 1) / 2),   // Apply `destRectModifier.y`, center `destRect` based on `tilesetData->animationSize.y` and `destRectModifier.h`
        utils::ftoi(level::data.tileDestSize.x * sTilesetData.animationSize.x * mDestRectModifier.w),
        utils::ftoi(level::data.tileDestSize.y * sTilesetData.animationSize.y * mDestRectModifier.h),
    };
}

template <typename T>
bool AbstractEntity<T>::isTargetWithinRange(SDL_Point const& targetDestCoords, std::pair<int, int> const& x_coords_lim, std::pair<int, int> const& y_coords_lim) {
    return (x_coords_lim.first == -1 || x_coords_lim.first <= targetDestCoords.x)
    && (x_coords_lim.second == -1 || targetDestCoords.x <= x_coords_lim.second)
    && (y_coords_lim.first == -1 || y_coords_lim.first <= targetDestCoords.y)
    && (y_coords_lim.second == -1 || targetDestCoords.y <= y_coords_lim.second);    
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
tile::Data_EntityTileset AbstractEntity<T>::sTilesetData;

template <typename T>
int AbstractEntity<T>::sID_Counter = 0;


/**
 * @note Explicit Template Instantiation.
 * @note Bear fruition from 8 hours of debugging and not less than 50 articles from StackOverflow and other places.
 * @warning Recommended implementation: derived abstract classes MUST provide similar implementation at the end of the corresponding source files.
*/
template class AbstractEntity<Player>;

template class AbstractEntity<Umbra>;
template class AbstractEntity<OmoriLightBulb>;
template class AbstractEntity<OmoriKeysWASD>;

template class AbstractEntity<PlaceholderInteractable>;
template class AbstractEntity<PlayerShadow>;
template class AbstractEntity<OmoriLaptop>;
template class AbstractEntity<OmoriMewO>;
template class AbstractEntity<OmoriCat_0>;
template class AbstractEntity<OmoriCat_1>;
template class AbstractEntity<OmoriCat_2>;
template class AbstractEntity<OmoriCat_3>;
template class AbstractEntity<OmoriCat_4>;
template class AbstractEntity<OmoriCat_5>;
template class AbstractEntity<OmoriCat_6>;
template class AbstractEntity<OmoriCat_7>;

template class AbstractEntity<PlaceholderTeleporter>;
template class AbstractEntity<RedHandThrone>;

template class AbstractEntity<Crab>;

template class AbstractEntity<Darkness>;
template class AbstractEntity<Slash>;
template class AbstractEntity<Claw>;
template class AbstractEntity<Meteor>;