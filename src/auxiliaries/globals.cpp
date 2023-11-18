#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <auxiliaries/globals.hpp>


/**
 * @brief Everything that is related to tiles.
*/
namespace tiledata {
    /**
     * @brief Simulate the `NULL` value.
    */
    BaseTilesetData::BaseTilesetData() : texture(nullptr), srcCount({0, 0}), srcSize({0, 0}), properties({}) {}

    /**
     * @brief Clean up resources to prevent memory leaks.
    */
    void BaseTilesetData::dealloc() {
        if (texture != nullptr) SDL_DestroyTexture(texture);
        *this = BaseTilesetData();   // Simulate `NULL`
    }

    TilelayerTilesetData::TilelayerTilesetData() : BaseTilesetData::BaseTilesetData(), firstGID(0) {}

    const std::unordered_map<std::string, AnimatedEntitiesTilesetData::AnimationType> AnimatedEntitiesTilesetData::animationTypeMapping = {
        {"animation-idle", AnimationType::IDLE},
        {"animation-attack", AnimationType::ATTACK},
        {"animation-blink", AnimationType::BLINK},
        {"animation-death", AnimationType::DEATH},
        {"animation-disappear", AnimationType::DISAPPEAR},
        {"animation-duck", AnimationType::DUCK},
        {"animation-jump", AnimationType::JUMP},
        {"animation-run", AnimationType::RUN},
        {"animation-walk", AnimationType::WALK},
    };
}

/**
 * @brief Represent data related to level-loading. Populated by `interface` and read by member classes.
*/
namespace leveldata {
    std::size_t TextureData::TextureData_Hasher::operator()(const TextureData& obj) const { return std::hash<int>()(obj.destCoords.x) ^ (std::hash<int>()(obj.destCoords.y) << 1); }

    bool TextureData::TextureData_Equality_Operator::operator()(const TextureData& first, const TextureData& second) const { return first.destCoords.x == second.destCoords.x && first.destCoords.y == second.destCoords.y; }

    bool TextureData::TextureData_Less_Than_Operator::operator()(const TextureData& first, const TextureData& second) const { return (first.destCoords.y < second.destCoords.y) || (first.destCoords.y == second.destCoords.y && first.destCoords.x < second.destCoords.x); }
}


/**
 * @brief The global namespace. Member variables accessible to all classes.
*/
namespace globals {
    /**
     * @brief Deallocate the tilesets.
     * @note Should be called when the program terminates.
    */
    void dealloc() {
        if (globals::renderer != nullptr) SDL_DestroyRenderer(globals::renderer);
        for (auto& tilesetData : globals::tilesetDataCollection) tilesetData.dealloc();
    }

    SDL_Renderer* renderer = nullptr;
    SDL_Point windowSize;
    SDL_Point TILE_SRC_SIZE;
    SDL_Point tileDestSize;
    SDL_Point tileDestCount;
    SDL_Point windowOffset;
    tiledata::TilelayerTilesetData::TilelayerTilesetDataCollection tilesetDataCollection;
    leveldata::LevelData currentLevelData;
}