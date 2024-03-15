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
    json data;
    utils::readJSON(config::interface::path, data);
    sLevelMap.load(data);
}

void IngameMapHandler::render() const {
    SDL_RenderCopy(globals::renderer, isOnGrayscale ? mGrayscaleTexture : mTexture, nullptr, nullptr);
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

    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET | SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC, mTextureSize.x, mTextureSize.y);

    renderToTexture();
}

void IngameMapHandler::onWindowChange() {
    #if defined(_WIN64) || defined(_WIN32) || defined(_WIN16)
    // Weird windows-specific bug, don't know how to fix, temporary patch adversely affects performance (which scales proportionally with level size)
    renderToTexture();
    #endif
}

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
void IngameMapHandler::loadLevel() const {
    auto kLevelPath = sLevelMap[mLevelName];
    if (!kLevelPath.has_value() || !std::filesystem::exists(kLevelPath.value())) return;
    
    json JSONLevelData;
    utils::readJSON(kLevelPath.value().string(), JSONLevelData);
    level::data.load(JSONLevelData);
}

void IngameMapHandler::renderToTexture() {
    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTexture);
    SDL_RenderClear(globals::renderer);

    renderBackground();
    renderLevelTilelayers();

    if (mGrayscaleTexture != nullptr) SDL_DestroyTexture(mGrayscaleTexture);
    mGrayscaleTexture = utils::createGrayscaleTexture(globals::renderer, mTexture, config::interface::grayscaleIntensity);

    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
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
*/
void IngameMapHandler::renderLevelTilelayers() const {
    SDL_Rect GID_SrcRect, GID_DestRect;
    SDL_Texture* GID_Texture = nullptr;   // Assign-only
    tile::GID prevGID = 0;

    GID_DestRect.x = GID_DestRect.y = 0;
    GID_DestRect.w = level::data.tileDestSize.x;
    GID_DestRect.h = level::data.tileDestSize.y;

    for (int y = 0; y < level::data.tileDestCount.y; ++y) {
        for (int x = 0; x < level::data.tileDestCount.x; ++x) {
            for (const auto& gid : level::data.tiles[y][x]) {
                if (!gid) continue;   // A GID value of `0` represents an "empty" tile i.e. associated with no tileset
                
                if (gid != prevGID) {   // Aims to reduce the number of calls to `Data_TilelayerTilesets::operator[]` which is essentially `std::find_if()` which is `O(n)` time complexity
                    auto tilesetData = level::data.tilesets[gid];   // Identify the tileset to which the GID belongs (layer-specific); O(n) time complexity
                    if (!tilesetData.has_value()) continue;   // GID is invalid

                    auto tilesetData_v = tilesetData.value();
                    if (tilesetData_v.getProperty("norender") == "true") continue;   // GID is for non-render purposes e.g. collision

                    GID_Texture = tilesetData_v.texture;
                    GID_SrcRect = {
                        ((gid - tilesetData_v.firstGID) % tilesetData_v.srcCount.x) * tilesetData_v.srcSize.x,
                        ((gid - tilesetData_v.firstGID) / tilesetData_v.srcCount.x) * tilesetData_v.srcSize.y,
                        tilesetData_v.srcSize.x,
                        tilesetData_v.srcSize.y,
                    };

                    prevGID = gid;
                }

                SDL_RenderCopy(globals::renderer, GID_Texture, &GID_SrcRect, &GID_DestRect);
            }

            GID_DestRect.x += GID_DestRect.w;
        }

        GID_DestRect.x = 0;
        GID_DestRect.y += GID_DestRect.h;
    }
}


level::Map IngameMapHandler::sLevelMap;