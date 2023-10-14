#pragma once

#include <array>
#include <unordered_map>

#include <auxiliaries/structs.h>


// Represents all states of the game.
enum class GameState {
    MENU,
    INGAME_PLAYING,
    INGAME_PAUSED,
    INGAME_DIALOGUE,
    INGAME_CUTSCENE,
    EXIT,
};

// Represents all "levels" in the game.
enum class Level {
    CHAPEL_OF_ANTICIPATION,
};

// Represents all spritesheets.
enum class TileSet {
    FLOOR_GRASS, FLOOR_STONE, PLANT_SHADOW, PLANT, PROPS, SHADOW, STRUCT, WALL,
};

// Represents all sprite types.
enum class TileType {
    // `FLOOR_GRASS` tileset
    FLOOR_GRASS_A, FLOOR_GRASS_B, FLOOR_GRASS_C, FLOOR_GRASS_D, FLOOR_GRASS_E, FLOOR_GRASS_F, FLOOR_GRASS_G, FLOOR_GRASS_H, FLOOR_GRASS_I, FLOOR_GRASS_J, FLOOR_GRASS_K, FLOOR_GRASS_L, FLOOR_GRASS_M, FLOOR_GRASS_N, FLOOR_GRASS_O, FLOOR_GRASS_P,
    FLOOR_FLOWER_A, FLOOR_FLOWER_B, FLOOR_FLOWER_C, FLOOR_FLOWER_D, FLOOR_FLOWER_E, FLOOR_FLOWER_F, FLOOR_FLOWER_G, FLOOR_FLOWER_H, FLOOR_FLOWER_I, FLOOR_FLOWER_J, FLOOR_FLOWER_K, FLOOR_FLOWER_L, FLOOR_FLOWER_M, FLOOR_FLOWER_N, FLOOR_FLOWER_O, FLOOR_FLOWER_P,
    FLOOR_PATH_A, FLOOR_PATH_B, FLOOR_PATH_C, FLOOR_PATH_D, FLOOR_PATH_E, FLOOR_PATH_F, FLOOR_PATH_G, FLOOR_PATH_H,
    FLOOR_PATH_EAST_A, FLOOR_PATH_EAST_B, FLOOR_PATH_EAST_C, FLOOR_PATH_EAST_D,
    FLOOR_PATH_WEST_A, FLOOR_PATH_WEST_B, FLOOR_PATH_WEST_C, FLOOR_PATH_WEST_D,
    FLOOR_PATH_SOUTH_A, FLOOR_PATH_SOUTH_B, FLOOR_PATH_SOUTH_C, FLOOR_PATH_SOUTH_D,
    FLOOR_PATH_NORTH_A, FLOOR_PATH_NORTH_B, FLOOR_PATH_NORTH_C, FLOOR_PATH_NORTH_D,
    FLOOR_PATH_NORTHWEST, FLOOR_PATH_NORTHEAST, FLOOR_PATH_SOUTHWEST, FLOOR_PATH_SOUTHEAST,
    FLOOR_PATH_EXCL_NORTHWEST, FLOOR_PATH_EXCL_NORTHEAST,
    // `WALL` tileset
    FOUNDATION_A_NORTHWEST, FOUNDATION_A_NORTH, FOUNDATION_A_NORTHEAST,
    FOUNDATION_A_WEST, FOUNDATION_A_EAST,
    FOUNDATION_A_SOUTHWEST_TOP, FOUNDATION_A_SOUTHWEST_BOTTOM, FOUNDATION_A_SOUTH_TOP, FOUNDATION_A_SOUTH_BOTTOM, FOUNDATION_A_SOUTHEAST_TOP, FOUNDATION_A_SOUTHEAST_BOTTOM,
    FOUNDATION_B_NORTHWEST_TOP, FOUNDATION_B_NORTHNORTHWEST_TOP, FOUNDATION_B_NORTH_TOP, FOUNDATION_B_NORTHNORTHEAST_TOP, FOUNDATION_B_NORTHEAST_TOP,
    FOUNDATION_B_NORTHWEST_BOTTOM, FOUNDATION_B_NORTHNORTHWEST_BOTTOM, FOUNDATION_B_NORTH_BOTTOM, FOUNDATION_B_NORTHNORTHEAST_BOTTOM, FOUNDATION_B_NORTHEAST_BOTTOM,
    FOUNDATION_B_WEST, FOUNDATION_B_EAST,
    FOUNDATION_B_SOUTHWEST, FOUNDATION_B_SOUTHSOUTHWEST, FOUNDATION_B_SOUTH, FOUNDATION_B_SOUTHSOUTHEAST, FOUNDATION_B_SOUTHEAST,
    // `STRUCT` tileset
    STAIRS_CRACKED_A_NORTHWEST, STAIRS_CRACKED_A_NORTHEAST, STAIRS_CRACKED_A_WEST, STAIRS_CRACKED_A_EAST, STAIRS_CRACKED_A_SOUTHWEST, STAIRS_CRACKED_A_SOUTHEAST,
    STAIRS_A_NORTHWEST, STAIRS_A_NORTHEAST, STAIRS_A_WEST, STAIRS_A_EAST, STAIRS_A_SOUTHWEST, STAIRS_A_SOUTHEAST,
};

// Represents dimensions of the window, which are constants. Resizability, as of this commit, is not available.
const int WINDOW_SIZE_X = 1280; const int WINDOW_SIZE_Y = 720;

// Represents the number of tiles per dimension.
const int TILEMAP_SIZE_X = 38; const int TILEMAP_SIZE_Y = 20;

// The original size of each sprite on spritesheets.
const int TILE_SRC_SIZE = 32;
// The desired size to render on the window.
const int TILE_DEST_SIZE = 32;

// The base folder containing all spritesheets.
const std::string base = "assets/graphics/sprites/";
// A shorthand for finding the associated spritesheet.
const std::unordered_map<TileSet, std::string> TILESET_PATHS = {
    {TileSet::FLOOR_GRASS, base + "environments/floor-grass.png"},
    {TileSet::FLOOR_STONE, base + "environments/floor-stone.png"},
    {TileSet::PLANT_SHADOW, base + "environments/plant-shadow.png"},
    {TileSet::PLANT, base + "environments/plant.png"},
    {TileSet::PROPS, base + "environments/props.png"},
    {TileSet::SHADOW, base + "environments/shadow.png"},
    {TileSet::STRUCT, base + "environments/struct.png"},
    {TileSet::WALL, base + "environments/wall.png"},
};

using TileMap = std::array<std::array<TileData, TILEMAP_SIZE_X>, TILEMAP_SIZE_Y>;
using TileMapped = std::pair<SDL_Texture*, SDL_Point>;
using TileMapping = std::unordered_map<TileType, TileMapped>;