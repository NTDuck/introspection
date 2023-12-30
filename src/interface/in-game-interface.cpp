#include <interface.hpp>

#include <filesystem>
#include <vector>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameInterface::IngameInterface(const level::LevelName levelName) : levelName(levelName) {}

void IngameInterface::initialize() {
    if (!std::filesystem::exists(config::interface::path)) return;
    utils::loadLevelsData(kLevelMapping);
}

/**
 * @brief Force load level after changes.
*/
void IngameInterface::changeLevel(const level::LevelName levelName_) {
    levelName = levelName_;
    onLevelChange();
}

/**
 * @brief Populate `level` members and re-render `texture`.
*/
void IngameInterface::onLevelChange() {
    SDL_SetRenderTarget(globals::renderer, texture);
    SDL_RenderClear(globals::renderer);

    loadLevel();

    renderBackground();
    renderLevelTiles();

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

void IngameInterface::onWindowChange() {
    AbstractInterface<IngameInterface>::onWindowChange();
    onLevelChange();
}

/**
 * @brief Populate `level` members with relevant data.
 * @note Should be called once during initialization or whenever `level` changes.
*/
void IngameInterface::loadLevel() {
    std::filesystem::path kLevelPath = config::path::assetTiled / kLevelMapping[levelName];
    if (!std::filesystem::exists(kLevelPath)) return;
    json data;
    utils::readJSON(kLevelPath.string(), data);

    // Several attributes shall be assumed e.g. orthogonal orientation, right-down renderorder, tilerendersize = grid
    utils::loadLevelData(globals::currentLevelData, data);

    // Reset `globals::tilelayerTilesetDataCollection`
    utils::loadTilesetsData(globals::renderer, globals::tilelayerTilesetDataCollection, data);
}

/**
 * @brief Fill the window with the tileset's black to achieve a seamless feel.
*/
void IngameInterface::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, globals::currentLevelData.backgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

/**
 * @brief Render the static portions of a level to `texture`.
*/
void IngameInterface::renderLevelTiles() const {
    tile::TilelayerRenderData::Collection tileRenderDataCollection(globals::tileDestCount.y, std::vector<tile::TilelayerRenderData>(globals::tileDestCount.x));

    // Populate render data 
    for (int y = 0; y < globals::tileDestCount.y; ++y) {
        for (int x = 0; x < globals::tileDestCount.x; ++x) {
            tile::TilelayerRenderData& data = tileRenderDataCollection[y][x];

            data.destRect.x = globals::tileDestSize.x * x + globals::windowOffset.x;
            data.destRect.y = globals::tileDestSize.y * y + globals::windowOffset.y;
            data.destRect.w = globals::tileDestSize.x;
            data.destRect.h = globals::tileDestSize.y;

            for (const auto& gid : globals::currentLevelData.tileCollection[y][x]) {
                // A GID value of `0` represents an "empty" tile i.e. associated with no tileset.
                if (!gid) continue;
                auto tilesetData = utils::getTilesetData(globals::tilelayerTilesetDataCollection, gid);

                // Identify the tileset to which the tile, per layer, belongs to.
                if (tilesetData == nullptr) continue;

                auto it = tilesetData->properties.find("norender");
                data.textures.emplace_back(it != tilesetData->properties.end() && it->second == "true" ? nullptr : tilesetData->texture);

                data.srcRects.push_back({
                    ((gid - tilesetData->firstGID) % tilesetData->srcCount.x) * tilesetData->srcSize.x,
                    ((gid - tilesetData->firstGID) / tilesetData->srcCount.x) * tilesetData->srcSize.y,
                    tilesetData->srcSize.x,
                    tilesetData->srcSize.y,
                });
            }
        }
    }

    // Render all tiles
    for (const auto& tileRenderDataVector : tileRenderDataCollection) {
        for (const auto& tileRenderData : tileRenderDataVector) {
            int size = tileRenderData.textures.size();
            for (int i = 0; i < size; ++i) {
                if (tileRenderData.textures[i] == nullptr) continue;
                SDL_RenderCopy(globals::renderer, tileRenderData.textures[i], &tileRenderData.srcRects[i], &tileRenderData.destRect);
            }
        }
    }
}


level::LevelMapping IngameInterface::kLevelMapping;