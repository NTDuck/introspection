#pragma once

#include <array>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


namespace utils {
    std::vector<SDL_Point> generateFilledIndices(SDL_Point start, SDL_Point stop);
    std::vector<SDL_Point> generateOutlinedIndices(SDL_Point start, SDL_Point stop);

    TileType getRandomTileType(TileType start, TileType stop);

    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, SDL_Point indices);
    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, std::vector<SDL_Point> indicesVector);
    void dePopulateTiles(TileMap& tiles, TileMapping tileMapping, std::vector<std::vector<SDL_Point>> indicesVectorVector);

    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, SDL_Point indices);
    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, std::vector<SDL_Point> indicesVector);
    void populateTiles(TileMap& tiles, TileMapping tileMapping, TileType type, std::vector<std::vector<SDL_Point>> indicesVectorVector);

    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, SDL_Point indices);
    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, std::vector<SDL_Point> indicesVector);
    void populateRandomTiles(TileMap& tiles, TileMapping tileMapping, std::pair<TileType, TileType> types, std::vector<std::vector<SDL_Point>> indicesVectorVector);

    void generateTerrainRect(TileMap& tiles, TileMapping tileMapping, std::pair<SDL_Point, SDL_Point> indices);
    void generateTerrainBridge(TileMap& tiles, TileMapping tileMapping, SDL_Point startIndices);
    void generateTerrainBridgeCracked(TileMap& tiles, TileMapping tileMapping, SDL_Point startIndices);
}