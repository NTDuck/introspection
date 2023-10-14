#include <array>
#include <vector>
#include <random>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <auxiliaries/utils.h>


namespace utils {
    /**
     * @brief Returns all indices within specified coordinates, ordereed.
    */
    std::vector<SDL_Point> generateFilledIndices(SDL_Point start, SDL_Point stop) {
        std::vector<SDL_Point> indices;
        for (int x = start.x; x <= stop.x; ++x) {
            for (int y = start.y; y <= stop.y; ++y) {
                indices.push_back({x, y});
            }
        }
        return indices;
    }

    /**
     * @brief Returns only outermost indices within specified coordinates, ordereed.
    */
    std::vector<SDL_Point> generateOutlinedIndices(SDL_Point start, SDL_Point stop) {
        std::vector<SDL_Point> indices;
        for (int y = start.y; y <= stop.y; ++y) indices.push_back({ start.x, y });
        for (int y = start.y; y <= stop.y; ++y) indices.push_back({ stop.x, y });
        for (int x = start.x + 1; x <= stop.x - 1; ++x) indices.push_back({ x, start.y });
        for (int x = start.x + 1; x <= stop.x - 1; ++x) indices.push_back({ x, stop.y });
        return indices;
    }

    /**
     * @brief Returns a random `TileType`.
    */
    TileType getRandomTileType(TileType start, TileType stop) {
        // Create & seed a random number engine.
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(static_cast<int>(start), static_cast<int>(stop));

        // Generate a random integer then cast it back to `TileType`.
        int randomInt = distribution(gen);
        return static_cast<TileType>(randomInt);
    }

    /**
     * @brief Reset the tile at specified indices to the default value.
    */
    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, SDL_Point indices) {
        auto& tile = tiles.at(indices.x).at(indices.y);

        tile.textures.clear();
        tile.srcRects.clear();
        tile.angle = 0;
        tile.flip = SDL_FLIP_NONE;
    }

    /**
     * @brief Reset the tile at specified indices to the default value.
    */
    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, std::vector<SDL_Point> indicesVector) {
        for (const auto& indices : indicesVector) utils::dePopulateTiles(tiles, tileMapping, indices);
    }

    /**
     * @brief Reset the tile at specified indices to the default value.
    */
    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, std::vector<std::vector<SDL_Point>> indicesVectorVector) {
        for (const auto& indicesVector : indicesVectorVector) utils::dePopulateTiles(tiles, tileMapping, indicesVector);
    }

    /**
     * @brief Set certain attributes to a tile based on `type`.
    */
    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, SDL_Point indices) {
        const TileMapped pair = tileMapping.find(type) -> second;
        SDL_Texture* texture = pair.first;
        const SDL_Point SRC_COORDS = pair.second;

        auto& tile = tiles.at(indices.x).at(indices.y);
        tile.textures.emplace_back(texture);

        SDL_Rect srcRect;
        srcRect.x = TILE_SRC_SIZE * SRC_COORDS.x;
        srcRect.y = TILE_SRC_SIZE * SRC_COORDS.y;
        srcRect.w = TILE_SRC_SIZE;
        srcRect.h = TILE_SRC_SIZE;
        tile.srcRects.emplace_back(srcRect);
    }

    /**
     * @brief Set certain attributes to a tile based on `type`.
    */
    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, std::vector<SDL_Point> indicesVector) {
        for (const auto& indices : indicesVector) utils::populateTiles(tiles, tileMapping, type, indices);
    }

    /**
     * @brief Set certain attributes to a tile based on `type`.
    */
    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, std::vector<std::vector<SDL_Point>> indicesVectorVector) {
        for (const auto& indicesVector : indicesVectorVector) utils::populateTiles(tiles, tileMapping, type, indicesVector);
    }

    /**
     * @brief Set certain attributes to a tile based on a `type` in the specified range.
    */
    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, SDL_Point indices) {
        TileType type = utils::getRandomTileType(types.first, types.second);
        utils::populateTiles(tiles, tileMapping, type, indices);
    }

    /**
     * @brief Set certain attributes to a tile based on a `type` in the specified range.
    */
    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, std::vector<SDL_Point> indicesVector) {
        for (const auto& indices : indicesVector) utils::populateRandomTiles(tiles, tileMapping, types, indices);
    }

    /**
     * @brief Set certain attributes to a tile based on a `type` in the specified range.
    */
    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, std::vector<std::vector<SDL_Point>> indicesVectorVector) {
        for (const auto& indicesVector : indicesVectorVector) utils::populateRandomTiles(tiles, tileMapping, types, indicesVector);
    }

    /**
     * @brief A shorthand for generating a rectangular terrain.
    */
    void generateTerrainRect(TileMap& tiles, TileMapping tileMapping, std::pair<SDL_Point, SDL_Point> indices) {
        SDL_Point* startIndices = &indices.first;
        SDL_Point* stopIndices = &indices.second;

        // Ensure available space for all components.
        if (startIndices -> x < 1 || startIndices -> y < 1 || startIndices -> x > TILEMAP_SIZE_Y - 4 || startIndices -> y > TILEMAP_SIZE_X - 2) return;
        if (stopIndices -> x < 1 || stopIndices -> y < 1 || stopIndices -> x > TILEMAP_SIZE_Y - 4 || stopIndices -> x > TILEMAP_SIZE_Y - 2) return;
        if (startIndices -> x > stopIndices -> x || startIndices -> y > stopIndices -> y) return;

        // Populate tiles in a specific order.
        utils::populateRandomTiles(tiles, tileMapping, {TileType::FLOOR_GRASS_A, TileType::FLOOR_GRASS_P}, utils::generateFilledIndices(*startIndices, *stopIndices));

        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTHWEST_TOP, {stopIndices -> x + 1, startIndices -> y});
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTHWEST_BOTTOM, {stopIndices -> x + 2, startIndices -> y});
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTH_TOP, utils::generateFilledIndices({stopIndices -> x + 1, startIndices -> y + 1}, {stopIndices -> x + 1, stopIndices -> y - 1}));
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTH_BOTTOM, utils::generateFilledIndices({stopIndices -> x + 2, startIndices -> y + 1}, {stopIndices -> x + 2, stopIndices -> y - 1}));
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTHEAST_TOP, {stopIndices -> x + 1, stopIndices -> y});
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_SOUTHEAST_BOTTOM, {stopIndices -> x + 2, stopIndices -> y});

        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_NORTHWEST, *startIndices);
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_NORTHEAST, {startIndices -> x, stopIndices -> y});

        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_NORTH, utils::generateFilledIndices({startIndices -> x, startIndices -> y + 1}, {startIndices -> x, stopIndices -> y - 1}));
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_WEST, utils::generateFilledIndices({startIndices -> x + 1, startIndices -> y}, {stopIndices -> x, startIndices -> y}));
        utils::populateTiles(tiles, tileMapping, TileType::FOUNDATION_A_EAST, utils::generateFilledIndices({startIndices -> x + 1, stopIndices -> y}, {stopIndices -> x, stopIndices -> y}));
    }

    /**
     * @brief A shorthand for generating a bridge.
     * @note `startIndices` denotes the farthest northwestern indices on the `tiles` that the "bridge" should occupy.
    */
    void generateTerrainBridge(TileMap& tiles, TileMapping tileMapping, SDL_Point startIndices) {
        if (startIndices.x < 1 || startIndices.y < 1 || startIndices.x > TILEMAP_SIZE_Y - 4 || startIndices.y > TILEMAP_SIZE_X - 3) return;
    
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_NORTHWEST, startIndices);
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_NORTHEAST, {startIndices.x, startIndices.y + 1});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_WEST, {startIndices.x + 1, startIndices.y});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_EAST, {startIndices.x + 1, startIndices.y + 1});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_SOUTHWEST, {startIndices.x + 2, startIndices.y});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_A_SOUTHEAST, {startIndices.x + 2, startIndices.y + 1});
    }

    /**
     * @brief A shorthand for generating a cracked bridge.
     * @note `startIndices` denotes the farthest northwestern indices on the `tiles` that the "bridge" should occupy.
    */
    void generateTerrainBridgeCracked(TileMap& tiles, TileMapping tileMapping, SDL_Point startIndices) {
        if (startIndices.x < 1 || startIndices.y < 1 || startIndices.x > TILEMAP_SIZE_Y - 4 || startIndices.y > TILEMAP_SIZE_X - 3) return;
    
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_NORTHWEST, startIndices);
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_NORTHEAST, {startIndices.x, startIndices.y + 1});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_WEST, {startIndices.x + 1, startIndices.y});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_EAST, {startIndices.x + 1, startIndices.y + 1});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_SOUTHWEST, {startIndices.x + 2, startIndices.y});
        utils::populateTiles(tiles, tileMapping, TileType::STAIRS_CRACKED_A_SOUTHEAST, {startIndices.x + 2, startIndices.y + 1});
    }
}