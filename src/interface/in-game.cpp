#include <interface.hpp>

#include <filesystem>
#include <vector>

#include <SDL.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


IngameInterface::IngameInterface(const level::LevelName levelName) : levelName(levelName), texture(nullptr) {}

IngameInterface::~IngameInterface() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
}

IngameInterface* IngameInterface::instantiate(const level::LevelName levelName) {
    if (instance == nullptr) instance = new IngameInterface(levelName);
    return instance;
}

void IngameInterface::initialize() {
    if (!std::filesystem::exists(globals::config::kConfigPathLevel)) return;
    utils::loadLevelsData(levelMapping);
}

void IngameInterface::deinitialize() {
    delete instance;
    instance = nullptr;
}

/**
 * @brief Render the texture to the window.
 * @note Overlapping is allowed.
*/
void IngameInterface::render() {
    SDL_RenderCopy(globals::renderer, texture, nullptr, nullptr);
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
    // NOT a good way of resizing things
    if (texture != nullptr) SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, globals::windowSize.x, globals::windowSize.y);

    // Last resort
    onLevelChange();
}

/**
 * @brief Populate `level` members with relevant data.
 * @note Should be called once during initialization or whenever `level` changes.
*/
void IngameInterface::loadLevel() {
    std::filesystem::path LEVEL_PATH = globals::config::kTiledAssetPath / levelMapping[levelName];
    if (!std::filesystem::exists(LEVEL_PATH)) return;
    json data;
    utils::readJSON(LEVEL_PATH.string(), data);

    // Several attributes shall be assumed e.g. orthogonal orientation, right-down renderorder, tilerendersize = grid
    utils::loadLevelData(globals::currentLevelData, data);

    // Reset `globals::tilelayerTilesetDataCollection`
    utils::loadTilesetsData(globals::renderer, globals::tilelayerTilesetDataCollection, data);
}

/**
 * @brief Fill the window with the tileset's black to achieve a seamless feel.
*/
void IngameInterface::renderBackground() {
    SDL_SetRenderDrawColor(globals::renderer, globals::currentLevelData.backgroundColor.r, globals::currentLevelData.backgroundColor.g, globals::currentLevelData.backgroundColor.b, globals::currentLevelData.backgroundColor.a);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

/**
 * @brief Render the static portions of a level to `texture`.
*/
void IngameInterface::renderLevelTiles() {
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
                tile::TilelayerTilesetData tilesetData;

                // Identify the tileset to which the tile, per layer, belongs to.
                tilesetData = utils::getTilesetData(globals::tilelayerTilesetDataCollection, gid);

                data.textures.emplace_back(tilesetData.properties["norender"] == "true" ? nullptr : tilesetData.texture);
                data.srcRects.push_back({
                    ((gid - tilesetData.firstGID) % tilesetData.srcCount.x) * tilesetData.srcSize.x,
                    ((gid - tilesetData.firstGID) / tilesetData.srcCount.x) * tilesetData.srcSize.y,
                    tilesetData.srcSize.x,
                    tilesetData.srcSize.y,
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


IngameInterface* IngameInterface::instance = nullptr;
level::LevelMapping IngameInterface::levelMapping;