#include <array>
#include <numeric>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <interface.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <auxiliaries/utils.h>


Interface::Interface(Level level) : level(level) {}

Interface::~Interface() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    for (auto& pair : spriteSheets) {
        if (pair.second != nullptr) SDL_DestroyTexture(pair.second);
    }
}

/**
 * @brief Initialize the interface.
*/
void Interface::init(SDL_Renderer* renderer) {
    loadSpriteSheets(renderer);

    setupTileMapping();
    setupLevelMapping();
}

/**
 * @brief Load required spritesheets specified in `TILESET_PATHS`.
*/
void Interface::loadSpriteSheets(SDL_Renderer* renderer) {
    for (const auto& pair : TILESET_PATHS) {
        spriteSheets[pair.first] = IMG_LoadTexture(renderer, pair.second.c_str());
    }
}

/**
 * @brief Associate each `TileType` with its respective coordinates on the respective spritesheet.
 * @warning This gibberish code significantly impacts readability, maintainability, and scalability. Like, just look at it. How horrible.
*/
void Interface::setupTileMapping() {
    tileMapping = {
        // `FLOOR_GRASS` tileset
        {TileType::FLOOR_GRASS_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 0}}},
        {TileType::FLOOR_GRASS_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 0}}},
        {TileType::FLOOR_GRASS_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 0}}},
        {TileType::FLOOR_GRASS_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 0}}},
        {TileType::FLOOR_GRASS_E, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 1}}},
        {TileType::FLOOR_GRASS_F, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 1}}},
        {TileType::FLOOR_GRASS_G, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 1}}},
        {TileType::FLOOR_GRASS_H, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 1}}},
        {TileType::FLOOR_GRASS_I, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 2}}},
        {TileType::FLOOR_GRASS_J, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 2}}},
        {TileType::FLOOR_GRASS_K, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 2}}},
        {TileType::FLOOR_GRASS_L, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 2}}},
        {TileType::FLOOR_GRASS_M, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 3}}},
        {TileType::FLOOR_GRASS_N, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 3}}},
        {TileType::FLOOR_GRASS_O, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 3}}},
        {TileType::FLOOR_GRASS_P, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 3}}},

        {TileType::FLOOR_FLOWER_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 0}}},
        {TileType::FLOOR_FLOWER_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 0}}},
        {TileType::FLOOR_FLOWER_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 0}}},
        {TileType::FLOOR_FLOWER_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 0}}},
        {TileType::FLOOR_FLOWER_E, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 1}}},
        {TileType::FLOOR_FLOWER_F, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 1}}},
        {TileType::FLOOR_FLOWER_G, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 1}}},
        {TileType::FLOOR_FLOWER_H, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 1}}},
        {TileType::FLOOR_FLOWER_I, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 2}}},
        {TileType::FLOOR_FLOWER_J, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 2}}},
        {TileType::FLOOR_FLOWER_K, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 2}}},
        {TileType::FLOOR_FLOWER_L, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 2}}},
        {TileType::FLOOR_FLOWER_M, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 3}}},
        {TileType::FLOOR_FLOWER_N, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 3}}},
        {TileType::FLOOR_FLOWER_O, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 3}}},
        {TileType::FLOOR_FLOWER_P, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 3}}},

        {TileType::FLOOR_PATH_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 4}}},
        {TileType::FLOOR_PATH_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 4}}},
        {TileType::FLOOR_PATH_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 5}}},
        {TileType::FLOOR_PATH_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 5}}},
        {TileType::FLOOR_PATH_E, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 6}}},
        {TileType::FLOOR_PATH_F, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 6}}},
        {TileType::FLOOR_PATH_G, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {0, 7}}},
        {TileType::FLOOR_PATH_H, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {1, 7}}},

        {TileType::FLOOR_PATH_EAST_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 4}}},
        {TileType::FLOOR_PATH_EAST_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 5}}},
        {TileType::FLOOR_PATH_EAST_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 6}}},
        {TileType::FLOOR_PATH_EAST_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {2, 7}}},

        {TileType::FLOOR_PATH_WEST_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 4}}},
        {TileType::FLOOR_PATH_WEST_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 5}}},
        {TileType::FLOOR_PATH_WEST_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 6}}},
        {TileType::FLOOR_PATH_WEST_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {3, 7}}},

        {TileType::FLOOR_PATH_SOUTH_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 4}}},
        {TileType::FLOOR_PATH_SOUTH_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 4}}},
        {TileType::FLOOR_PATH_SOUTH_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 4}}},
        {TileType::FLOOR_PATH_SOUTH_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 4}}},

        {TileType::FLOOR_PATH_NORTH_A, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 5}}},
        {TileType::FLOOR_PATH_NORTH_B, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 5}}},
        {TileType::FLOOR_PATH_NORTH_C, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 5}}},
        {TileType::FLOOR_PATH_NORTH_D, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 5}}},

        {TileType::FLOOR_PATH_NORTHWEST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 6}}},
        {TileType::FLOOR_PATH_NORTHEAST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 6}}},
        {TileType::FLOOR_PATH_SOUTHWEST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {6, 6}}},
        {TileType::FLOOR_PATH_SOUTHEAST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {7, 6}}},

        {TileType::FLOOR_PATH_EXCL_NORTHWEST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {4, 7}}},
        {TileType::FLOOR_PATH_EXCL_NORTHEAST, {spriteSheets.find(TileSet::FLOOR_GRASS) -> second, {5, 7}}},

        // `WALL` tileset
        {TileType::FOUNDATION_A_NORTHWEST, {spriteSheets.find(TileSet::WALL) -> second, {1, 1}}},
        {TileType::FOUNDATION_A_NORTH, {spriteSheets.find(TileSet::WALL) -> second, {2, 1}}},
        {TileType::FOUNDATION_A_NORTHEAST, {spriteSheets.find(TileSet::WALL) -> second, {3, 1}}},
        {TileType::FOUNDATION_A_WEST, {spriteSheets.find(TileSet::WALL) -> second, {1, 2}}},
        {TileType::FOUNDATION_A_EAST, {spriteSheets.find(TileSet::WALL) -> second, {3, 2}}},
        {TileType::FOUNDATION_A_SOUTHWEST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {1, 3}}},
        {TileType::FOUNDATION_A_SOUTHWEST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {1, 4}}},
        {TileType::FOUNDATION_A_SOUTH_TOP, {spriteSheets.find(TileSet::WALL) -> second, {2, 3}}},
        {TileType::FOUNDATION_A_SOUTH_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {2, 4}}},
        {TileType::FOUNDATION_A_SOUTHEAST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {3, 3}}},
        {TileType::FOUNDATION_A_SOUTHEAST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {3, 4}}},

        {TileType::FOUNDATION_B_NORTHWEST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {4, 1}}},
        {TileType::FOUNDATION_B_NORTHNORTHWEST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {5, 1}}},
        {TileType::FOUNDATION_B_NORTH_TOP, {spriteSheets.find(TileSet::WALL) -> second, {6, 1}}},
        {TileType::FOUNDATION_B_NORTHNORTHEAST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {7, 1}}},
        {TileType::FOUNDATION_B_NORTHEAST_TOP, {spriteSheets.find(TileSet::WALL) -> second, {8, 1}}},
        {TileType::FOUNDATION_B_NORTHWEST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {4, 2}}},
        {TileType::FOUNDATION_B_NORTHNORTHWEST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {5, 2}}},
        {TileType::FOUNDATION_B_NORTH_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {6, 2}}},
        {TileType::FOUNDATION_B_NORTHNORTHEAST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {7, 2}}},
        {TileType::FOUNDATION_B_NORTHEAST_BOTTOM, {spriteSheets.find(TileSet::WALL) -> second, {8, 2}}},
        {TileType::FOUNDATION_B_WEST, {spriteSheets.find(TileSet::WALL) -> second, {4, 3}}},
        {TileType::FOUNDATION_B_EAST, {spriteSheets.find(TileSet::WALL) -> second, {8, 3}}},
        {TileType::FOUNDATION_B_SOUTHWEST, {spriteSheets.find(TileSet::WALL) -> second, {4, 4}}},
        {TileType::FOUNDATION_B_SOUTHSOUTHWEST, {spriteSheets.find(TileSet::WALL) -> second, {5, 4}}},
        {TileType::FOUNDATION_B_SOUTH, {spriteSheets.find(TileSet::WALL) -> second, {6, 4}}},
        {TileType::FOUNDATION_B_SOUTHSOUTHEAST, {spriteSheets.find(TileSet::WALL) -> second, {7, 4}}},
        {TileType::FOUNDATION_B_SOUTHEAST, {spriteSheets.find(TileSet::WALL) -> second, {8, 4}}},

        // `STRUCT` tileset
        {TileType::STAIRS_A_NORTHWEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 1}}},
        {TileType::STAIRS_A_NORTHEAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 1}}},
        {TileType::STAIRS_A_WEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 2}}},
        {TileType::STAIRS_A_EAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 2}}},
        {TileType::STAIRS_A_SOUTHWEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 3}}},
        {TileType::STAIRS_A_SOUTHEAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 3}}},

        {TileType::STAIRS_CRACKED_A_NORTHWEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 5}}},
        {TileType::STAIRS_CRACKED_A_NORTHEAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 5}}},
        {TileType::STAIRS_CRACKED_A_WEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 6}}},
        {TileType::STAIRS_CRACKED_A_EAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 6}}},
        {TileType::STAIRS_CRACKED_A_SOUTHWEST, {spriteSheets.find(TileSet::STRUCT) -> second, {1, 7}}},
        {TileType::STAIRS_CRACKED_A_SOUTHEAST, {spriteSheets.find(TileSet::STRUCT) -> second, {2, 7}}},
    };
}

/**
 * @brief Populate the `std::unordered_map` used for level-loading by assigning labmda functions associated with respective methods.
 * @note Remember to bind `this` else the following exception occurs: `the enclosing-function 'this' cannot be referenced in a lambda body unless it is in the capture listC/C++(1738)`.
*/
void Interface::setupLevelMapping() {
    levelMapping = {
        {Level::CHAPEL_OF_ANTICIPATION, [this]() {loadLevelChapelOfAnticipation();}},
    };
}

/**
 * @brief Populate all tiles with non-level-specific attributes that will remain constant throughout the game.
*/
void Interface::setupTiles(const SDL_Point WINDOW_SIZE) {
    const int TILE_DEST_SIZE = std::min(WINDOW_SIZE.x / TILEMAP_SIZE_X, WINDOW_SIZE.y / TILEMAP_SIZE_Y);
    const SDL_Point OFFSET = {
        (WINDOW_SIZE.x - TILEMAP_SIZE_X * TILE_DEST_SIZE) / 2,
        (WINDOW_SIZE.y - TILEMAP_SIZE_Y * TILE_DEST_SIZE) / 2,
    };

    for (int y = 0; y < TILEMAP_SIZE_Y; ++y) {
        for (int x = 0; x < TILEMAP_SIZE_X; ++x) {
            auto& tile = tileMap.at(y).at(x);

            tile.destRect.x = TILE_DEST_SIZE * x + OFFSET.x;
            tile.destRect.y = TILE_DEST_SIZE * y + OFFSET.y;
            tile.destRect.w = TILE_DEST_SIZE;
            tile.destRect.h = TILE_DEST_SIZE;
        }
    }
}

/**
 * @brief Prepare/re-prepare `texture` for rendering.
*/
void Interface::blit(SDL_Renderer* renderer, const SDL_Point WINDOW_SIZE) {
    texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, WINDOW_SIZE.x, WINDOW_SIZE.y);
    SDL_SetRenderTarget(renderer, texture);

    setupTiles(WINDOW_SIZE);
    loadLevel();

    renderBackground(renderer, WINDOW_SIZE);
    renderTiles(renderer);

    SDL_SetRenderTarget(renderer, nullptr);
}

/**
 * @brief Render `levelTexture` to the window.
 * @note This method should be called rarely rather than in every loop.
 * @note Overlapping is allowed.
*/
void Interface::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

/**
 * @brief Simulate a "transparent"-textured background- a darkend chessboard.
*/
void Interface::renderBackground(SDL_Renderer* renderer, const SDL_Point WINDOW_SIZE) {
    // Simulate a background with "transparent" texture - crossboard with 2 colors.
    const int BACKGROUND_RECT_SIZE = std::gcd(WINDOW_SIZE.x, WINDOW_SIZE.y);
    const int BACKGROUND_RECT_COUNT_X = WINDOW_SIZE.x / BACKGROUND_RECT_SIZE;
    const int BACKGROUND_RECT_COUNT_Y = WINDOW_SIZE.y / BACKGROUND_RECT_SIZE;

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
 * @brief Render all tiles based on data specified in `tileMap`.
*/
void Interface::renderTiles(SDL_Renderer* renderer) {
    for (auto& row : tileMap) {
        for (auto& tile : row) {
            int size = tile.textures.size();
            if (size < 1) continue;
            for (int i = 0; i < size; ++i) SDL_RenderCopyEx(renderer, tile.textures.at(i), &tile.srcRects.at(i), &tile.destRect, tile.angle, nullptr, tile.flip);
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
    // Simulate de-initializing all tiles.
    for (int y = 0; y < TILEMAP_SIZE_Y; ++y) {
        for (int x = 0; x < TILEMAP_SIZE_X; ++x) {
            utils::dePopulateTiles(tileMap, tileMapping, {y, x});
        }
    }

    // Call the associated level-loading method.
    auto it = levelMapping.find(level);
    it -> second();
}

/**
 * @brief Create the environments of level "Chapel of Anticipation".
 * @warning Using `std::array` yields `too many initializer valuesC/C++(146)`.
*/
void Interface::loadLevelChapelOfAnticipation() {
    // Generate main chunks of terrain.
    utils::generateTerrainRect(tileMap, tileMapping, {{3, 1}, {16, 36}});
    utils::generateTerrainRect(tileMap, tileMapping, {{1, 11}, {12, 14}});
    utils::generateTerrainRect(tileMap, tileMapping, {{1, 23}, {12, 26}});
    utils::generateTerrainRect(tileMap, tileMapping, {{1, 15}, {4, 22}});
    utils::generateTerrainBridge(tileMap, tileMapping, {5, 16});
    utils::generateTerrainBridgeCracked(tileMap, tileMapping, {5, 20});

    // Make slight modifications to the terrain chunks.
    utils::dePopulateTiles(tileMap, tileMapping, {utils::generateFilledIndices({1, 14}, {4, 15}), utils::generateFilledIndices({1, 22}, {4, 23})});
    utils::populateRandomTiles(tileMap, tileMapping, {TileType::FLOOR_GRASS_A, TileType::FLOOR_GRASS_P}, {utils::generateFilledIndices({1, 14}, {4, 15}), utils::generateFilledIndices({1, 22}, {4, 23})});
    utils::populateTiles(tileMap, tileMapping, TileType::FOUNDATION_A_NORTH, {{1, 14}, {1, 15}, {1, 22}, {1, 23}});
    utils::populateTiles(tileMap, tileMapping, TileType::FOUNDATION_A_SOUTH_TOP, {{5, 15}, {5, 22}});
    utils::populateTiles(tileMap, tileMapping, TileType::FOUNDATION_A_SOUTH_BOTTOM, {{6, 15}, {6, 22}});

    // Make not-so-slight modifications to the terrain chunks.
    utils::dePopulateTiles(tileMap, tileMapping, {5, 14});
    utils::dePopulateTiles(tileMap, tileMapping, {5, 23});
    utils::populateRandomTiles(tileMap, tileMapping, {TileType::FLOOR_GRASS_A, TileType::FLOOR_GRASS_P}, {5, 14});
    utils::populateRandomTiles(tileMap, tileMapping, {TileType::FLOOR_GRASS_A, TileType::FLOOR_GRASS_P}, {5, 23});
    utils::populateTiles(tileMap, tileMapping, TileType::FOUNDATION_B_NORTHWEST_TOP, {5, 14});
    utils::populateTiles(tileMap, tileMapping, TileType::FOUNDATION_B_NORTHEAST_TOP, {5, 23});
}