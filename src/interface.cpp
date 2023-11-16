#include <filesystem>
#include <vector>

#include <SDL.h>

#include <interface.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


Interface::Interface(const std::string levelName_) : levelName(levelName_) {}

Interface::~Interface() { if (texture != nullptr) SDL_DestroyTexture(texture); }

/**
 * @brief Initialize the interface.
*/
void Interface::init() {
    if (!std::filesystem::exists(globals::config::LEVELS_PATH)) return;
    utils::loadLevelsData(levelMapping);
}

/**
 * @brief Render `levelTexture` to the window.
 * @note Overlapping is allowed.
*/
void Interface::render() {
    SDL_RenderCopy(globals::renderer, texture, nullptr, nullptr);
}

/**
 * @brief Force load level after changes.
*/
void Interface::changeLevel(const std::string levelName_) {
    levelName = levelName_;
    onLevelChange();
}

/**
 * @brief Populate `globals::levelData` members and re-render `texture`.
*/
void Interface::onLevelChange() {
    SDL_SetRenderTarget(globals::renderer, texture);
    SDL_RenderClear(globals::renderer);

    loadLevel();

    renderBackgroundToTexture();
    renderLevelTilesToTexture();

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

/**
 * @brief Unnecessary perhaps?
*/
void Interface::onWindowChange() {
    // NOT a good way of resizing things
    if (texture != nullptr) SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, globals::windowSize.x, globals::windowSize.y);

    // Don't even bother
    onLevelChange();
}

/**
 * @brief Populate `globals::levelData` members with relevant data.
 * @note Should be called once during initialization or whenever `level` changes.
*/
void Interface::loadLevel() {
    std::filesystem::path LEVEL_PATH = globals::config::TILED_ASSETS_PATH / levelMapping[levelName];
    if (!std::filesystem::exists(LEVEL_PATH)) return;
    json data;
    utils::readJSON(LEVEL_PATH.string(), data);

    // Several attributes shall be assumed e.g. orthogonal orientation, right-down renderorder, tilerendersize = grid
    utils::loadLevelData(globals::currentLevelData, data);

    // Reset `globals::TILESET_COLLECTION`
    utils::loadTilesetsData(globals::renderer, globals::tilesetDataCollection, data);
}

/**
 * @brief Fill the window with the tileset's black to achieve a seamless feel.
*/
void Interface::renderBackgroundToTexture() {
    // const int BACKGROUND_RECT_SIZE = std::gcd(globals::WINDOW_SIZE.x, globals::WINDOW_SIZE.y);
    // const int BACKGROUND_RECT_COUNT_X = globals::WINDOW_SIZE.x / BACKGROUND_RECT_SIZE;
    // const int BACKGROUND_RECT_COUNT_Y = globals::WINDOW_SIZE.y / BACKGROUND_RECT_SIZE;

    // std::vector<SDL_Rect> lighterRects;
    // std::vector<SDL_Rect> darkerRects;

    // for (int y = 0; y < BACKGROUND_RECT_COUNT_Y; ++y) {
    //     for (int x = 0; x < BACKGROUND_RECT_COUNT_X; ++x) {
    //         SDL_Rect backgroundRect;
    //         backgroundRect.x = BACKGROUND_RECT_SIZE * x;
    //         backgroundRect.y = BACKGROUND_RECT_SIZE * y;
    //         backgroundRect.w = BACKGROUND_RECT_SIZE;
    //         backgroundRect.h = BACKGROUND_RECT_SIZE;
    //         if ((x + y) & 1) darkerRects.emplace_back(backgroundRect); else lighterRects.emplace_back(backgroundRect);
    //     }
    // }
    
    // SDL_SetRenderDrawColor(globals::renderer, 0x1a, 0x11, 0x10, SDL_ALPHA_OPAQUE);
    // for (const auto& lighterRect : lighterRects) SDL_RenderFillRect(globals::renderer, &lighterRect);

    // SDL_SetRenderDrawColor(globals::renderer, 0x10, 0x0c, 0x08, SDL_ALPHA_OPAQUE);
    // for (const auto& darkerRect : darkerRects) SDL_RenderFillRect(globals::renderer, &darkerRect);

    SDL_SetRenderDrawColor(globals::renderer, globals::currentLevelData.backgroundColor.r, globals::currentLevelData.backgroundColor.g, globals::currentLevelData.backgroundColor.b, globals::currentLevelData.backgroundColor.a);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

/**
 * @brief Render the static portions of a level to `texture`.
*/
void Interface::renderLevelTilesToTexture() {
    tiledata::TileRenderData::TileRenderDataCollection tileRenderDataCollection(globals::tileDestCount.y, std::vector<tiledata::TileRenderData>(globals::tileDestCount.x));

    // Populate render data 
    for (int y = 0; y < globals::tileDestCount.y; ++y) {
        for (int x = 0; x < globals::tileDestCount.x; ++x) {
            tiledata::TileRenderData& data = tileRenderDataCollection[y][x];

            data.destRect.x = globals::tileDestSize.x * x + globals::windowOffset.x;
            data.destRect.y = globals::tileDestSize.y * y + globals::windowOffset.y;
            data.destRect.w = globals::tileDestSize.x;
            data.destRect.h = globals::tileDestSize.y;

            for (const auto& gid : globals::currentLevelData.tileCollection[y][x]) {
                // A GID value of `0` represents an "empty" tile i.e. associated with no tileset.
                if (!gid) continue;
                tiledata::TilelayerTilesetData tilesetData;

                // Identify the tileset to which the tile, per layer, belongs to.
                tilesetData = utils::getTilesetData(globals::tilesetDataCollection, gid);

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