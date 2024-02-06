#include <auxiliaries.hpp>

#include <unordered_map>
#include <SDL.h>


const std::unordered_map<std::string, tile::EntitiesTilesetData::AnimationType> tile::EntitiesTilesetData::kAnimationTypeConversionMapping = {
    { "animation-idle", AnimationType::kIdle },
    { "animation-attack", AnimationType::kAttack },
    { "animation-blink", AnimationType::kBlink },
    { "animation-death", AnimationType::kDeath },
    { "animation-disappear", AnimationType::kDisappear },
    { "animation-duck", AnimationType::kDuck },
    { "animation-jump", AnimationType::kJump },
    { "animation-run", AnimationType::kRun },
    { "animation-walk", AnimationType::kWalk },
    { "animation-damaged", AnimationType::kDamaged },
};

const std::unordered_map<std::string, level::LevelName> level::kLevelNameConversionMapping = {
    { "level-equilibrium", LevelName::kLevelEquilibrium },
    { "level-valley-of-despair", LevelName::kLevelValleyOfDespair },
};


SDL_Renderer* globals::renderer = nullptr;
SDL_Point globals::windowSize;
SDL_Point globals::tileDestSize;
SDL_Point globals::tileDestCount;
SDL_Point globals::windowOffset;
SDL_Point globals::mouseState;
tile::TilelayerTilesetData::Collection globals::tilelayerTilesetDataCollection;
level::LevelData globals::currentLevelData;
GameState globals::state = GameState::kMenu;


/**
 * @brief Deallocate resources.
 * @note Should be called when the program terminates.
*/
void globals::deinitialize() {
    if (globals::renderer != nullptr) {
        SDL_DestroyRenderer(globals::renderer);
        globals::renderer = nullptr;
    }
    
    for (auto& tilesetData : globals::tilelayerTilesetDataCollection) tilesetData.deinitialize();
}

void EntitySecondaryStats::initialize(EntityPrimaryStats const& entityPrimaryStats) {
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

/**
 * @brief Update both entities' secondary stats after entity `active` registered an attack onto entity `passive`.
*/
void EntitySecondaryStats::resolve(EntitySecondaryStats const& active, EntitySecondaryStats& passive) {
    passive.HP -= std::round(active.PhysicalDamage * (1 - passive.PhysicalDefense) * (utils::generateRandomBinary(active.CriticalChance) ? active.CriticalDamage : 1));
    passive.HP -= std::round(active.MagicDamage * (1 - passive.MagicDefense) * (utils::generateRandomBinary(active.CriticalChance) ? active.CriticalDamage : 1));
}