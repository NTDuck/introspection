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
    utils::fetch(config::interface::levelPath, data);
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
    utils::fetch(kLevelPath.value().string(), JSONLevelData);
    level::data.load(JSONLevelData);
}

void IngameMapHandler::loadProgressFromStorage(json const& saveData) {
    #define IMPL(ln) \
    case static_cast<unsigned int>(ln):\
        changeLevel(ln);\
        break;

    switch (static_cast<unsigned int>(saveData["level"])) {
        IMPL(level::Name::kLevelWhiteSpace)
        IMPL(level::Name::kLevelBegin)
        IMPL(level::Name::kLevelForest_0)
        IMPL(level::Name::kLevelForest_1)
        IMPL(level::Name::kLevelForest_2)
        IMPL(level::Name::kLevelForest_3)
        IMPL(level::Name::kLevelForest_4)
        IMPL(level::Name::kLevelDeprecatedTutorial_0)
        IMPL(level::Name::kLevelDeprecatedTutorial_1)
        IMPL(level::Name::kLevelAnte)
        IMPL(level::Name::kLevelPaene)
        IMPL(level::Name::kLevelUmbra)
        IMPL(level::Name::kLevelBreakroomInitial)
        IMPL(level::Name::kLevelBedroom)

        default: break;
    }
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

    utils::LRUCache<tile::GID, tile::Data_TilelayerTileset> cache(config::interface::LRUCacheSize);   // Aims to reduce the number of calls to `Data_TilelayerTilesets::operator[]` which is essentially `std::lower_bound` which is `O(log(n))` time complexity

    GID_DestRect.x = GID_DestRect.y = 0;
    GID_DestRect.w = level::data.tileDestSize.x;
    GID_DestRect.h = level::data.tileDestSize.y;

    tile::Data_TilelayerTileset tilesetData;

    for (int y = 0; y < level::data.tileDestCount.y; ++y) {
        for (int x = 0; x < level::data.tileDestCount.x; ++x) {
            for (const auto& gid : level::data.tiles[y][x]) {
                if (!gid) continue;   // A GID value of `0` represents an "empty" tile i.e. associated with no tileset

                auto cache_result = cache.at(gid);   // O(1) time complexity

                if (!cache_result.has_value()) {   // GID not recently used
                    auto tilesetData_o = level::data.tilesets[gid];   // O(log(n)) time complexity
                    if (!tilesetData_o.has_value()) continue;   // GID is invalid
                    tilesetData = tilesetData_o.value();
                    cache.insert(gid, tilesetData);   // Store to cache
                } else tilesetData = cache_result.value();   // O(1) time complexity

                if (tilesetData.getProperty("norender") == "true") continue;   // GID is for non-render purposes e.g. collision

                GID_Texture = tilesetData.texture;
                GID_SrcRect = {
                    ((gid - tilesetData.firstGID) % tilesetData.srcCount.x) * tilesetData.srcSize.x,
                    ((gid - tilesetData.firstGID) / tilesetData.srcCount.x) * tilesetData.srcSize.y,
                    tilesetData.srcSize.x,
                    tilesetData.srcSize.y,
                };

                SDL_RenderCopy(globals::renderer, GID_Texture, &GID_SrcRect, &GID_DestRect);
            }

            GID_DestRect.x += GID_DestRect.w;
        }

        GID_DestRect.x = 0;
        GID_DestRect.y += GID_DestRect.h;
    }

    cache.clear();
}


level::Map IngameMapHandler::sLevelMap;