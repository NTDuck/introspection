#include <interface.hpp>

#include <filesystem>
#include <vector>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameMapHandler::IngameMapHandler(const level::Name levelName) : AbstractInterface<IngameMapHandler>(), mLevelName(levelName) {}

IngameMapHandler::~IngameMapHandler() {
    if (mGrayscaleTexture != nullptr) {
        SDL_DestroyTexture(mGrayscaleTexture);
        mGrayscaleTexture = nullptr;
    }
}

void IngameMapHandler::initialize() {
    // utils::loadLevelsData(sLevelMap);
    json data;
    utils::readJSON(config::interface::path, data);
    sLevelMap.load(data);
}

void IngameMapHandler::render() const {
    SDL_RenderCopy(globals::renderer, (isOnGrayscale ? mGrayscaleTexture : mTexture), nullptr, nullptr);
}

/**
 * @brief Populate `level` members and re-render `texture`.
 * @note The `grayscaleTexture` block must be at THAT exact location i.e. before resetting render-target else undefined behaviour would be encountered.
*/
void IngameMapHandler::onLevelChange() {
    loadLevel();

    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTextureSize = {
        level::data.tileDestCount.x * level::data.tileDestSize.x,
        level::data.tileDestCount.y * level::data.tileDestSize.y,
    };
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextureSize.x, mTextureSize.y);

    SDL_SetRenderTarget(globals::renderer, mTexture);
    SDL_RenderClear(globals::renderer);

    renderBackground();
    renderLevelTiles();

    if (mGrayscaleTexture != nullptr) SDL_DestroyTexture(mGrayscaleTexture);
    mGrayscaleTexture = utils::createGrayscaleTexture(globals::renderer, mTexture, config::interface::grayscaleIntensity);

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

/**
 * @note Last resort.
*/
void IngameMapHandler::onWindowChange() { onLevelChange(); }

void IngameMapHandler::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case ~config::Key::kIngameGrayscaleToggle:
            if (event.type != SDL_KEYDOWN) break;
            isOnGrayscale ^= true;
            break;

        default: break;
    }
}

/**
 * @brief Force load level after changes.
*/
void IngameMapHandler::changeLevel(const level::Name levelName_) {
    mLevelName = levelName_;
    onLevelChange();
}

/**
 * @brief Populate `level` members with relevant data.
 * @note Should be called once during initialization or whenever `level` changes.
*/
void IngameMapHandler::loadLevel() {
    std::filesystem::path kLevelPath = sLevelMap[mLevelName];
    if (!std::filesystem::exists(kLevelPath)) return;
    
    json JSONLevelData;
    utils::readJSON(kLevelPath.string(), JSONLevelData);
    level::data.load(JSONLevelData);
}

/**
 * @brief Fill the window with the tileset's black to achieve a seamless feel.
*/
void IngameMapHandler::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, level::data.backgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

/**
 * @brief Render the static portions of a level to `texture`.
 * @note Needs optimization. Perhaps load only 1 `Data_Tile_RenderOnly` then immediately render then deallocate?
*/
void IngameMapHandler::renderLevelTiles() const {
    tile::Data_Tile_RenderOnly::Collection tileRenderDataCollection(level::data.tileDestCount.y, std::vector<tile::Data_Tile_RenderOnly>(level::data.tileDestCount.x));

    // Populate render data 
    for (int y = 0; y < level::data.tileDestCount.y; ++y) {
        for (int x = 0; x < level::data.tileDestCount.x; ++x) {
            tile::Data_Tile_RenderOnly& data = tileRenderDataCollection[y][x];

            data.destRect.x = level::data.tileDestSize.x * x;
            data.destRect.y = level::data.tileDestSize.y * y;
            data.destRect.w = level::data.tileDestSize.x;
            data.destRect.h = level::data.tileDestSize.y;

            for (const auto& gid : level::data.tiles[y][x]) {
                // A GID value of `0` represents an "empty" tile i.e. associated with no tileset.
                if (!gid) continue;
                auto tilesetData = level::data.tilesets[gid];

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


level::Map IngameMapHandler::sLevelMap;