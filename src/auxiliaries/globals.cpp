#include <auxiliaries/globals.hpp>

#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>


void EntitySecondaryStats::initialize(const EntityPrimaryStats& entityPrimaryStats) {
    HP = 10 + double(entityPrimaryStats.Vigor) / 3;
    FP = 10 + double(entityPrimaryStats.Mind) / 3;
    Stamina = 10 + double(entityPrimaryStats.Dexterity) / 10;
    Poise = 50 + double(entityPrimaryStats.Dexterity) / 20;

    PhysicalDefense = double(entityPrimaryStats.Endurance) / 200;
    MagicDefense = double(entityPrimaryStats.Intelligence) / 200;
    PhysicalDamage = 2 + double(entityPrimaryStats.Strength) / 10;
    MagicDamage = 3 + double(entityPrimaryStats.Faith) / 10;

    CriticalChance = double(entityPrimaryStats.Arcane) / 15;
    if (CriticalChance > 1) CriticalChance = 1;
    CriticalDamage = 3;
}

void tile::BaseTilesetData::deinitialize() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
}

const std::unordered_map<std::string, tile::EntitiesTilesetData::AnimationType> tile::EntitiesTilesetData::kAnimationTypeConversionMapping = {
    {"animation-idle", AnimationType::kIdle},
    {"animation-attack", AnimationType::kAttack},
    {"animation-blink", AnimationType::kBlink},
    {"animation-death", AnimationType::kDeath},
    {"animation-disappear", AnimationType::kDisappear},
    {"animation-duck", AnimationType::kDuck},
    {"animation-jump", AnimationType::kJump},
    {"animation-run", AnimationType::kRunning},
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

tile::NextAnimationData::NextAnimationData(AnimationType animationType) : animationType(animationType) {}

/**
 * @brief Update `instance` based on `pendingAnimationType`.
*/
void tile::NextAnimationData::update(NextAnimationData*& instance, const tile::EntitiesTilesetData::AnimationType pendingAnimationType) {
    if (instance == nullptr) {
        instance = new tile::NextAnimationData(pendingAnimationType);
        return;
    }
    
    // if (instance->isExecuting) return;

    // Update existing instance based on priority
    // Current priority: `kDamaged` > `kAttack`
    if (instance->animationType == AnimationType::kDamaged && pendingAnimationType == AnimationType::kAttack) return;
    instance->animationType = pendingAnimationType;
}

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