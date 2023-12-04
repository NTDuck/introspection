#include <auxiliaries/globals.hpp>

#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>


tile::BaseTilesetData::BaseTilesetData() : texture(nullptr), srcCount({0, 0}), srcSize({0, 0}), properties({}) {}   // Simulate `NULL` value

void tile::BaseTilesetData::deinitialize() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    *this = BaseTilesetData();
}

tile::TilelayerTilesetData::TilelayerTilesetData() : BaseTilesetData::BaseTilesetData(), firstGID(0) {}

const std::unordered_map<std::string, tile::AnimatedEntitiesTilesetData::AnimationType> tile::AnimatedEntitiesTilesetData::kAnimationTypeConversionMapping = {
    {"animation-idle", AnimationType::kIdle},
    {"animation-attack", AnimationType::kAttack},
    {"animation-blink", AnimationType::kBlink},
    {"animation-death", AnimationType::kDeath},
    {"animation-disappear", AnimationType::kDisappear},
    {"animation-duck", AnimationType::kDuck},
    {"animation-jump", AnimationType::kJump},
    {"animation-run", AnimationType::kRun},
    {"animation-walk", AnimationType::kWalk},
    {"animation-damaged", AnimationType::kDamaged},
};


const std::unordered_map<std::string, level::LevelName> level::kLevelNameConversionMapping = {
    {"level-equilibrium", LevelName::kLevelEquilibrium},
    {"level-valley-of-despair", LevelName::kLevelValleyOfDespair},
};

std::size_t level::EntityLevelData::Hasher::operator()(const EntityLevelData& obj) const { return std::hash<int>()(obj.destCoords.x) ^ (std::hash<int>()(obj.destCoords.y) << 1); }

bool level::EntityLevelData::Equality_Operator::operator()(const EntityLevelData& first, const EntityLevelData& second) const { return first.destCoords.x == second.destCoords.x && first.destCoords.y == second.destCoords.y; }

bool level::EntityLevelData::Less_Than_Operator::operator()(const EntityLevelData& first, const EntityLevelData& second) const { return (first.destCoords.y < second.destCoords.y) || (first.destCoords.y == second.destCoords.y && first.destCoords.x < second.destCoords.x); }


/**
 * @brief Deallocate resources.
 * @note Should be called when the program terminates.
*/
void globals::deinitialize() {
    if (globals::renderer != nullptr) SDL_DestroyRenderer(globals::renderer);
    for (auto& tilesetData : globals::tilelayerTilesetDataCollection) tilesetData.deinitialize();
}

SDL_Renderer* globals::renderer = nullptr;
SDL_Point globals::windowSize;
SDL_Point globals::tileDestSize;
SDL_Point globals::tileDestCount;
SDL_Point globals::windowOffset;
tile::TilelayerTilesetData::Collection globals::tilelayerTilesetDataCollection;
level::LevelData globals::currentLevelData;