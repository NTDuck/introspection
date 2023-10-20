#pragma once

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>


/**
 * @brief Represents all game states. Should be used (i.e. read and updated) closely with the main control flow.
*/
enum class GameState {
    MENU,
    INGAME_PLAYING,
    INGAME_PAUSED,
    INGAME_DIALOGUE,
    INGAME_CUTSCENE,
    EXIT,
};

/**
 * @brief Represents all levels. Should also cover sub-levels.
*/
enum class Level {
    CHAPEL_OF_ANTICIPATION,
};

/**
 * @brief Represents all tilesets/spritesheets.
*/
enum class TileSet {
    FLOOR_GRASS, WALL, STRUCT,
};


/**
 * @brief Represents SDL flags and other configurations used in the initialization of SDL subsystems.
 * 
 * @param init any of the following: `SDL_INIT_TIMER` (timer subsystem), `SDL_INIT_AUDIO` (audio subsystem), `SDL_INIT_VIDEO` (video subsystem - automatically initializes the events subsystem), `SDL_INIT_JOYSTICK` (joystick subsystem - automatically initializes the events subsystem), `SDL_INIT_HAPTIC` (haptic i.e. force feedback subsystem), `SDL_INIT_GAMECONTROLLER` (controller subsystem - automatically initializes the joystick subsystem), `SDL_INIT_EVENTS` (events subsystem), `SDL_INIT_EVERYTHING` (all of the above subsystems), `SDL_INIT_NOPARACHUTE` (compatibility - will be ignored), or multiple OR'd together. Determines which SDL subsystem(s) to initialize. Used in `SDL_Init()`.  @see https://wiki.libsdl.org/SDL2/SDL_Init
 * @param window an enumeration instance of `SDL_WindowFlags` or multiple OR'd together. Used in `SDL_CreateWindow()`. @see https://wiki.libsdl.org/SDL2/SDL_WindowFlags
 * @param image an enumeration instance of `IMG_InitFlags` or multiple OR'd together. Used by `IMG_Init()`. @see https://wiki.libsdl.org/SDL2_image/IMG_Init
 * @param renderer an enumeration instance of `SDL_RendererFlags` or multiple OR'd together. Determines the rendering context. Used in `SDL_CreateRenderer()`. @see https://wiki.libsdl.org/SDL2/SDL_RendererFlags
 * @param hints maps a hint with an associated value. Used in `SDL_SetHint()` and `SDL_SetHintWithPriority()`.
 * 
 * @note Multiple flags are combinable via bitwise OR `|`.
*/
struct Flags {
    Uint32 init;
    Uint32 window;
    Uint32 renderer;   // try SDL_RENDERER_PRESENTVSYNC
    int image;
    std::unordered_map<std::string, std::string> hints;
};

/**
 * @brief Represents data about the tilesets required for rendering.
 * 
 * @param texture The loaded `SDL_Texture`. Usually the tileset itself.
 * @param path The path to the tileset, preferably relative. @todo Should switch to `std::filesystem::path` for, hopefully, cross-platform compatibility.
 * @param firstGID The GID corresponding to the first tile in the set. Regulated by the level JSON, should not be manually set or modified otherwise.
 * @param TILE_SRC_COUNT The number of tiles per dimension in the tileset.
 * @param TILE_SRC_SIZE The maximum dimension of tiles in the tileset.
*/
struct TilesetData {
    SDL_Texture* texture;
    std::string path;
    int firstGID;
    SDL_Point TILE_SRC_COUNT;
    SDL_Point TILE_SRC_SIZE;
};

/**
 * @brief Represents data about the tilesets required for rendering.
 * 
 * @param textures An ordered, dynamic iterable of `SDL_Texture`. Each determines the tilesets to which the tile belongs, per layer.
 * @param srcRects An ordered, dynamic iterable of `SDL_Rect` with the same size as `textures`. `srcRects[i]` corresponds to `textures[i]` and should be used in conjunction to determine the specific portion of the tileset used for rendering.
 * @param destRect The specific portion of the window on which the tile is rendered. Independent of the tileset, its value is determined only with `globals::TILE_DEST_SIZE` and `globals::OFFSET`.
 * @note Should only be instantiated upon rendering.
*/
struct TileRenderData {
    std::vector<SDL_Texture*> textures;
    std::vector<SDL_Rect> srcRects;
    SDL_Rect destRect;
};


using json = nlohmann::json;

/**
 * @brief The base unit used for rendering. Contains only non-negative integers known as GIDs (Global Tile IDs) in Tiled terminology.
 * @see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data
*/
using Tile = std::vector<int>;

/**
 * @brief A 2D iterable of `Tile`. Represents/simulates a level/sub-level. Should be treated as an `std::array`.
 * @todo Separate into layers for dynamic alterations mid-program.
*/
using TileCollection = std::vector<std::vector<Tile>>;

/**
 * @brief A 2D iterable of `TileRenderData`. `TileRenderDataCollection[i][j]` corresponds to `TileCollection[i][j]` and should be used in conjunction to determine the rendering data.
*/
using TileRenderDataCollection = std::vector<std::vector<TileRenderData>>;

/**
 * @brief Maps a `TileSet` enumeration with the associated data required for rendering.
 * @note Initialization should be made once in `globals::init()`, should be treated as a constant henceforth.
 * @see <src/auxiliaries/globals.cpp> globals::init() (namespace method)
*/
using TilesetDataMapping = std::unordered_map<TileSet, TilesetData>;


namespace globals {
    /**
     * @brief The size of the in-game window.
     * @note Its value is assigned after game initialization, then updated whenever the window is resized.
     * @see <src/game.cpp> Game.blit() (classmethod)
    */
    extern SDL_Point WINDOW_SIZE;

    /**
     * @brief The maximum dimension of each tile on the window.
    */
    extern SDL_Point TILE_DEST_SIZE;

    /**
     * @brief The maximum number of tiles per dimension on the window.
    */
    extern SDL_Point TILE_DEST_COUNT;

    /**
     * @brief Intends to "center" the rendereed texture.
    */
    extern SDL_Point OFFSET;

    /**
     * @brief An unordered map that maps `TileSet` with the associated data. Required for many texture-related operations.
     * @note Its value is assigned during initialization and should be treated as a constant henceforth.
    */
    extern TilesetDataMapping TILESET_MAPPING;

    void init(SDL_Renderer* renderer);
    void cleanup();
}