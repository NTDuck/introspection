#include <meta.hpp>

#include <filesystem>
#include <unordered_set>
#include <type_traits>

#include <SDL.h>
#include <SDL_image.h>
#include <pugixml/pugixml.hpp>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class T>
bool AbstractEntity<T>::operator==(const AbstractEntity<T>& other) const {
    return destCoords == other.destCoords;
}

template <class T>
bool AbstractEntity<T>::operator<(const AbstractEntity<T>& other) const {
    return destCoords < other.destCoords;
}

template <class T>
std::size_t AbstractEntity<T>::PointerHasher::operator()(const T* pointer) const {
    return std::hash<int>()(pointer->destCoords.x) ^ std::hash<int>()(pointer->destCoords.y);
}

template <class T>
bool AbstractEntity<T>::PointerEqualityOperator::operator()(const T* first, const T* second) const {
    return *first == *second;
}

/**
 * @brief Create an instance of derived class `T` and register to `instanceMapping`.
*/
template <class T>
T* AbstractEntity<T>::instantiate(SDL_Point destCoords) {
    auto instance = new T;
    instance->destCoords = destCoords;
    instances.emplace(instance);   // `if constexpr(requires{ instance->destCoords; })`
    return instance;
}

template <class T>
AbstractEntity<T>::AbstractEntity()
    : destRectModifier(globals::config::kDefaultEntityDestRectModifier),
    angle(0), center(nullptr), flip(SDL_FLIP_NONE) {
    srcRect.w = tilesetData->srcSize.x * tilesetData->animationSize.x;
    srcRect.h = tilesetData->srcSize.y * tilesetData->animationSize.y;
}

/**
 * @brief Delete an instance of derived class `T` and remove from `instanceMapping`.
*/
template <class T>
AbstractEntity<T>::~AbstractEntity() {
    instances.erase(static_cast<T*>(this));
}

/**
 * @see <utils.h> utils::loadTilesetsData
*/
template <class T>
void AbstractEntity<T>::initialize() {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(kTilesetPath.c_str());
    // delete tilesetPath;
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`
    
    tilesetData = new tile::AnimatedEntitiesTilesetData;
    tilesetData->initialize(document, globals::renderer);
}

template <class T>
void AbstractEntity<T>::deinitialize() {
    // for (auto& instance : instances) delete instance;
    instances.clear();
    tilesetData->deinitialize();
    tilesetData = nullptr;
}

/**
 * @brief Set color modulation on the texture of derived class `T`.
*/
template <class T>
void AbstractEntity<T>::setRGB(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(tilesetData->texture, r, g, b);
}

/**
 * @brief Enable blending on the texture of derived class `T`.
 * @param blendMode the blending mode. Defaults to `SDL_BLENDMODE_BLEND`.
 * @note Required for alpha modulation.
 * @see https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
template <class T>
void AbstractEntity<T>::setBlending(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(tilesetData->texture, blendMode);
}

/**
 * @brief Set alpha modulation on the texture of derived class `T`.
 * @param alpha the alpha value. `0` represents full transparency, `255` represents full opacity.
*/
template <class T>
void AbstractEntity<T>::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(tilesetData->texture, alpha);
}

/**
 * @brief Set both color modulation and alpha modulation on the texture of derived class `T`.
 * @param col represents a standard RGBA value.
*/
template <class T>
void AbstractEntity<T>::setRGBA(SDL_Color& color) {
    setRGB(color.r, color.g, color.b);
    setBlending();
    setAlpha(color.a);
}

/**
 * @brief Clear `instanceMapping` then call `onLevelChange()` method on every instance of derived class `T`.
 * @todo Allow only `level::EntityLevelData` and its subclasses. Try `<type_traits>` and `<concepts>`.
*/
template <class T>
template <typename LevelData>
void AbstractEntity<T>::callOnEach_onLevelChange(const typename level::EntityLevelData::Collection<LevelData>& entityLevelDataCollection) {
    // for (auto& instance : instances) delete instance;
    instances.clear();

    for (const auto& entityLevelData : entityLevelDataCollection) {
        auto instance = instantiate(entityLevelData.destCoords);
        instance->onLevelChange(entityLevelData);
    }
}

/**
 * @brief Render the current sprite to the window.
 * @note Recommended implementation: this method requires `destRect` and `srcRect` to be set properly prior to being called.
*/
template <class T>
void AbstractEntity<T>::render() const {
    SDL_RenderCopyEx(globals::renderer, tilesetData->texture, &srcRect, &destRect, angle, center, flip);
}

/**
 * @brief Update data members that depend on window dimensions.
*/
template <class T>
void AbstractEntity<T>::onWindowChange() {
    destRect = getDestRectFromCoords(destCoords);
}

/**
 * @brief Update relevant data members on entering new level.
 * @note Recommended implementation: this method should be defined by derived classes.
*/
template <class T>
void AbstractEntity<T>::onLevelChange(const level::EntityLevelData& entityLevelData) {
    secondaryStats.initialize(primaryStats);   // Prevent resetting secondary stats on player entity
    destCoords = entityLevelData.destCoords;
}

/**
 * Adjust `destRect` based on `tilesetData->animationSize` and `destRectModifier`.
 * @return A `SDL_Rect` representing the position of the instance of derived class `T`, relative to the window.
 * @see https://stackoverflow.com/questions/3127962/c-float-to-int
*/
template <class T>
SDL_Rect AbstractEntity<T>::getDestRectFromCoords(const SDL_Point& coords) {
    return {
        coords.x * globals::tileDestSize.x + globals::windowOffset.x
        + utils::convertFloatToInt(destRectModifier.x * globals::tileDestSize.x)
        - (tilesetData->animationSize.x - 1) / 2 * globals::tileDestSize.x
        - utils::convertFloatToInt(globals::tileDestSize.x * tilesetData->animationSize.x * (destRectModifier.w - 1) / 2),   // Apply `destRectModifier.x`, center `destRect` based on `tilesetData->animationSize.x` and `destRectModifier.w`
        coords.y * globals::tileDestSize.y + globals::windowOffset.y + utils::convertFloatToInt(destRectModifier.y * globals::tileDestSize.y) - (tilesetData->animationSize.y - 1) / 2 * globals::tileDestSize.y - utils::convertFloatToInt(globals::tileDestSize.y * tilesetData->animationSize.y * (destRectModifier.h - 1) / 2),   // Apply `destRectModifier.y`, center `destRect` based on `tilesetData->animationSize.y` and `destRectModifier.h`
        utils::convertFloatToInt(globals::tileDestSize.x * tilesetData->animationSize.x * destRectModifier.w),
        utils::convertFloatToInt(globals::tileDestSize.y * tilesetData->animationSize.y * destRectModifier.h),
    };
}

/**
 * Store pointers of instances of derived class `T`. Based on `destCoords`. Useful for determining interactions between different instances of different derived classes e.g. entity collision.
*/
template <class T>
std::unordered_set<T*, typename AbstractEntity<T>::PointerHasher, typename AbstractEntity<T>::PointerEqualityOperator> AbstractEntity<T>::instances;

template <class T>
tile::AnimatedEntitiesTilesetData* AbstractEntity<T>::tilesetData = nullptr;


/**
 * @note Explicit Template Instantiation.
 * @note Bear fruition from 8 hours of debugging and not less than 50 articles from StackOverflow and other places.
 * @warning Recommended implementation: derived abstract classes MUST provide similar implementation at the end of the corresponding source files.
*/
template class AbstractEntity<Player>;
template class AbstractEntity<Teleporter>;
template class AbstractEntity<Slime>;

template void AbstractEntity<Teleporter>::callOnEach_onLevelChange<level::TeleporterLevelData>(const level::EntityLevelData::Collection<level::TeleporterLevelData>& entityLevelDataCollection);
template void AbstractEntity<Slime>::callOnEach_onLevelChange<level::SlimeLevelData>(const level::EntityLevelData::Collection<level::SlimeLevelData>& entityLevelDataCollection);