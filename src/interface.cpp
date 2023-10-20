#include <iostream>
#include <array>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <interface.h>

#include <auxiliaries/globals.h>
#include <auxiliaries/utils.h>


Interface::Interface(Level level) : level(level) {}

Interface::~Interface() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
}

/**
 * @brief Initialize the interface.
*/
void Interface::init(SDL_Renderer* renderer) {
    setupLevelMapping();
}


/**
 * @brief Populate the `std::unordered_map` used for level-loading by assigning labmda functions associated with respective methods.
 * @note Remember to bind `this` else the following exception occurs: `the enclosing-function 'this' cannot be referenced in a lambda body unless it is in the capture listC/C++(1738)`.
*/
void Interface::setupLevelMapping() {
    levelMapping = {
        {Level::CHAPEL_OF_ANTICIPATION, "assets/.tiled/level0.json"},
    };
}

/**
 * @brief Prepare/re-prepare `texture` for rendering.
*/
void Interface::blit(SDL_Renderer* renderer) {
    texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, globals::WINDOW_SIZE.x, globals::WINDOW_SIZE.y);
    SDL_SetRenderTarget(renderer, texture);

    loadLevel();

    renderBackground(renderer);
    renderLevel(renderer);

    SDL_SetRenderTarget(renderer, nullptr);
}

/**
 * @brief Render `levelTexture` to the window.
 * @note Overlapping is allowed.
*/
void Interface::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

/**
 * @brief Simulate an arbitrary chessboard-resembling background. Still better than nothing.
*/
void Interface::renderBackground(SDL_Renderer* renderer) {
    const int BACKGROUND_RECT_SIZE = std::gcd(globals::WINDOW_SIZE.x, globals::WINDOW_SIZE.y);
    const int BACKGROUND_RECT_COUNT_X = globals::WINDOW_SIZE.x / BACKGROUND_RECT_SIZE;
    const int BACKGROUND_RECT_COUNT_Y = globals::WINDOW_SIZE.y / BACKGROUND_RECT_SIZE;

    std::vector<SDL_Rect> lighterRects;
    std::vector<SDL_Rect> darkerRects;

    for (int y = 0; y < BACKGROUND_RECT_COUNT_Y; ++y) {
        for (int x = 0; x < BACKGROUND_RECT_COUNT_X; ++x) {
            SDL_Rect backgroundRect;
            backgroundRect.x = BACKGROUND_RECT_SIZE * x;
            backgroundRect.y = BACKGROUND_RECT_SIZE * y;
            backgroundRect.w = BACKGROUND_RECT_SIZE;
            backgroundRect.h = BACKGROUND_RECT_SIZE;
            if ((x + y) & 1) darkerRects.emplace_back(backgroundRect); else lighterRects.emplace_back(backgroundRect);
        }
    }
    
    SDL_SetRenderDrawColor(renderer, 0x1a, 0x11, 0x10, SDL_ALPHA_OPAQUE);
    for (const auto& lighterRect : lighterRects) SDL_RenderFillRect(renderer, &lighterRect);

    SDL_SetRenderDrawColor(renderer, 0x10, 0x0c, 0x08, SDL_ALPHA_OPAQUE);
    for (const auto& darkerRect : darkerRects) SDL_RenderFillRect(renderer, &darkerRect);
}

/**
 * @brief Render the environments of a level.
*/
void Interface::renderLevel(SDL_Renderer* renderer) {
    TileRenderDataCollection tileRenderDataCollection(globals::TILE_DEST_COUNT.y, std::vector<TileRenderData>(globals::TILE_DEST_COUNT.x));

    // Populate render data 
    for (int y = 0; y < globals::TILE_DEST_COUNT.y; ++y) {
        for (int x = 0; x < globals::TILE_DEST_COUNT.x; ++x) {
            TileRenderData& data = tileRenderDataCollection[y][x];

            data.destRect.x = globals::TILE_DEST_SIZE.x * x + globals::OFFSET.x;
            data.destRect.y = globals::TILE_DEST_SIZE.y * y + globals::OFFSET.y;
            data.destRect.w = globals::TILE_DEST_SIZE.x;
            data.destRect.h = globals::TILE_DEST_SIZE.y;

            for (const auto& gid : tileCollection[y][x]) {
                // A GID value of `0` represents an "empty" tile i.e. associated with no tileset.
                if (!gid) continue;
                TilesetData tilesetData;

                // Identify the tileset to which the tile, per layer, belongs to.
                for (const auto& pair : globals::TILESET_MAPPING) {
                    if (pair.second.firstGID <= gid && gid < pair.second.firstGID + pair.second.TILE_SRC_COUNT.x * pair.second.TILE_SRC_COUNT.y) {
                        tilesetData = pair.second;
                        break;
                    }
                }

                data.textures.emplace_back(tilesetData.texture);

                SDL_Rect srcRect;
                srcRect.x = ((gid - tilesetData.firstGID) % tilesetData.TILE_SRC_COUNT.x) * tilesetData.TILE_SRC_SIZE.x;
                srcRect.y = ((gid - tilesetData.firstGID) / tilesetData.TILE_SRC_COUNT.x) * tilesetData.TILE_SRC_SIZE.y;
                srcRect.w = tilesetData.TILE_SRC_SIZE.x;
                srcRect.h = tilesetData.TILE_SRC_SIZE.y;
                data.srcRects.emplace_back(srcRect);
            }
        }
    }

    // Render all tiles.
    for (const auto& tileRenderDataVector : tileRenderDataCollection) {
        for (const auto& tileRenderData : tileRenderDataVector) {
            int size = tileRenderData.textures.size();
            for (int i = 0; i < size; ++i) {
                SDL_RenderCopy(renderer, tileRenderData.textures[i], &tileRenderData.srcRects[i], &tileRenderData.destRect);
            }
        }
    }
}

/**
 * @brief Force load level after changes.
*/
void Interface::changeLevel(Level _level) {
    level = _level;
    loadLevel();
}

/**
 * @brief Create the environments of a level.
 * @note Recommended order: ground -> walls/structs -> props (-> entities)
 * @note Should be called during initialization or whenever `level` changes.
*/
void Interface::loadLevel() {
    json levelData;
    utils::readJSON(levelMapping.find(level) -> second, levelData);

    // several attrs will be assumed: orthogonal orientation, right-down renderorder

    for (auto& tileRow : tileCollection) for (auto& tile : tileRow) tile.clear();

    // update global variables
    globals::TILE_DEST_COUNT = {levelData["width"], levelData["height"]};
    globals::TILE_DEST_SIZE = {globals::WINDOW_SIZE.x / globals::TILE_DEST_COUNT.x, globals::WINDOW_SIZE.y / globals::TILE_DEST_COUNT.y};
    globals::OFFSET = {
        (globals::WINDOW_SIZE.x - globals::TILE_DEST_COUNT.x * globals::TILE_DEST_SIZE.x) / 2,
        (globals::WINDOW_SIZE.y - globals::TILE_DEST_COUNT.y * globals::TILE_DEST_SIZE.y) / 2,
    };

    tileCollection.resize(globals::TILE_DEST_COUNT.y);
    for (auto& tileRow : tileCollection) tileRow.resize(globals::TILE_DEST_COUNT.x);

    // emplace gids into Tiles
    for (const auto& layer : levelData["layers"]) {
        json data = layer["data"];
        for (int y = 0; y < globals::TILE_DEST_COUNT.y; ++y) {
            for (int x = 0; x < globals::TILE_DEST_COUNT.x; ++x) {
                tileCollection[y][x].emplace_back(data[y * globals::TILE_DEST_COUNT.x + x]);
            }
        }
    }
}