#include <auxiliaries/globals.hpp>

#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>


namespace tile {
    BaseTilesetData::BaseTilesetData() : texture(nullptr), srcCount({0, 0}), srcSize({0, 0}), properties({}) {}   // Simulate `NULL` value

    void BaseTilesetData::deinitialize() {
        if (texture != nullptr) SDL_DestroyTexture(texture);
        *this = BaseTilesetData();
    }

    TilelayerTilesetData::TilelayerTilesetData() : BaseTilesetData::BaseTilesetData(), firstGID(0) {}

    const std::unordered_map<std::string, AnimatedEntitiesTilesetData::AnimationType> AnimatedEntitiesTilesetData::kAnimationTypeConversionMapping = {
        {"animation-idle", AnimationType::kIdle},
        {"animation-attack", AnimationType::kAttack},
        {"animation-blink", AnimationType::kBlink},
        {"animation-death", AnimationType::kDeath},
        {"animation-disappear", AnimationType::kDisappear},
        {"animation-duck", AnimationType::kDuck},
        {"animation-jump", AnimationType::kJump},
        {"animation-run", AnimationType::kRun},
        {"animation-walk", AnimationType::kWalk},
    };
}

namespace level {
    const std::unordered_map<std::string, LevelName> kLevelNameConversionMapping = {
        {"level-equilibrium", LevelName::kLevelEquilibrium},
        {"level-valley-of-despair", LevelName::kLevelValleyOfDespair},
    };

    std::size_t EntityLevelData::Hasher::operator()(const EntityLevelData& obj) const { return std::hash<int>()(obj.destCoords.x) ^ (std::hash<int>()(obj.destCoords.y) << 1); }

    bool EntityLevelData::Equality_Operator::operator()(const EntityLevelData& first, const EntityLevelData& second) const { return first.destCoords.x == second.destCoords.x && first.destCoords.y == second.destCoords.y; }

    bool EntityLevelData::Less_Than_Operator::operator()(const EntityLevelData& first, const EntityLevelData& second) const { return (first.destCoords.y < second.destCoords.y) || (first.destCoords.y == second.destCoords.y && first.destCoords.x < second.destCoords.x); }
}


namespace globals {
    /**
     * @brief Deallocate resources.
     * @note Should be called when the program terminates.
    */
    void deinitialize() {
        if (globals::renderer != nullptr) SDL_DestroyRenderer(globals::renderer);
        for (auto& tilesetData : globals::tilelayerTilesetDataCollection) tilesetData.deinitialize();
    }

    SDL_Renderer* renderer = nullptr;
    SDL_Point windowSize;
    SDL_Point tileDestSize;
    SDL_Point tileDestCount;
    SDL_Point windowOffset;
    tile::TilelayerTilesetData::Collection tilelayerTilesetDataCollection;
    level::LevelData currentLevelData;
}