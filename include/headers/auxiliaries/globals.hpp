#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>
#include <pugixml/pugixml.hpp>


using json = nlohmann::json;


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
struct InitFlags {
    Uint32 init;
    Uint32 window;
    Uint32 renderer;   // try SDL_RENDERER_PRESENTVSYNC
    int image;
    std::unordered_map<std::string, std::string> hints;
};


/**
 * @brief Everything that is related to tiles.
*/
namespace tiledata {
    /**
     * @brief The base unit used for rendering. Contains only non-negative integers known as GIDs (Global Tile IDs) in Tiled terminology.
     * @see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data
    */
    using Tile = std::vector<int>;

    /**
     * @brief A 2D iterable of `Tile`. Should be treated as an `std::array`.
     * @todo Separate into layers for dynamic alterations mid-program.
    */
    using TileCollection = std::vector<std::vector<Tile>>;

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

        /**
         * @brief A 2D iterable of `TileRenderData`. `TileRenderDataCollection[i][j]` corresponds to `TileCollection[i][j]` and should be used in conjunction to determine the rendering data.
        */
        using TileRenderDataCollection = std::vector<std::vector<TileRenderData>>;
    };

    /**
     * @brief Represent a tileset's data.
     * @note `init()` method of this and derived classes are implemented elsewhere.
    */
    struct BaseTilesetData {
        /**
         * @brief The loaded `SDL_Texture`. Usually the tileset itself.
        */
        SDL_Texture* texture;

        /**
         * @brief The number of tiles per dimension in the tileset.
        */
        SDL_Point srcCount;

        /**
         * @brief The maximum dimension of tiles in the tileset.
        */
        SDL_Point srcSize;

        /**
         * @brief A mapping that stores properties of Tiled standard type e.g. string, bool, int.
         * @note `"norender"` This tileset will not be rendered.
         * @note `"collision"` This tileset controls collision between entities.
        */
        std::unordered_map<std::string, std::string> properties;

        BaseTilesetData();
        void init(pugi::xml_document& document, SDL_Renderer* renderer);
        void dealloc();
    };

    /**
     * @brief Represent data of a tileset used for tilelayers.
    */
    struct TilelayerTilesetData : public BaseTilesetData {
        /**
         * @brief The GID corresponding to the first tile in the set. Regulated by the level JSON, should not be manually set or modified otherwise.
        */
        int firstGID;
        void init(const json& tileset, SDL_Renderer* renderer);
        TilelayerTilesetData();

        /**
         * @brief Maps a `TileSet` enumeration with the associated data required for rendering.
         * @note Updates once per `<src/interface.cpp> Interface.loadLevel()` call, should be treated as a constant otherwise.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        using TilelayerTilesetDataCollection = std::vector<TilelayerTilesetData>;
    };

    /**
     * @brief Represent data of a tileset used for entities and animated objects.
     * @note Default values are provided in case of missing/corrupted data.
    */
    struct AnimatedEntitiesTilesetData : public BaseTilesetData {
        /**
         * @brief Represent animation types as `enum` instead of `std::string` for maintainability.
        */
        enum class AnimationType {
            IDLE, ATTACK, BLINK, DEATH, DISAPPEAR, DUCK, JUMP, RUN, WALK,
        };

        /**
         * @brief Support conversion from tileset's `std::string` to usable `AnimationType`.
        */
        static const std::unordered_map<std::string, AnimationType> animationTypeMapping;

        /**
         * @brief Represent data of an animation i.e. a series of sprites.
        */
        struct Animation {
            int startGID = 0;
            int stopGID = 0;
            bool isPermanent = false;
        };

        /**
         * @brief Maps the animation type with its respective data.
         * @todo Consider implementing `enum` instead of `std::string`.
         * @todo Optimize retrieval.
        */
        std::unordered_map<AnimationType, Animation> animationMapping;

        /**
         * @brief Indicates the number of frames a sprite should last before switching to the next. Should be treated as a constant.
        */
        int animationUpdateRate = 64;

        /**
         * @brief Indicates the ratio between the size of one single animation/sprite and the size of a "tile" on the spritesheet per dimension.
         * @note Should be applied alongside `globals::tileDestSize`.
        */
        SDL_Point animationSize = {1, 1};

        void init(pugi::xml_document& document, SDL_Renderer* renderer);
    };

};

/**
 * @brief Represent data related to level-loading. Populated by `interface` and read by member classes.
*/
namespace leveldata {
    /**
     * @brief Maps a level's name with its corresponding relative file path. Initialized only once in interface initialization and should henceforth be treated as a constant.
     * @note For optimized memory usage, this approach does not encapsulate `Level` (struct), instead `globals::currentLevel` is loaded via `Interface.loadLevel()` based on file path.
     * @note Switch to a different data structure should more complexity be involved.
    */
    using LevelMapping = std::unordered_map<std::string, std::string>;

    struct TextureData {
        // Virtual destructor, required for polymorphism
        virtual ~TextureData() = default;

        struct TextureData_Hasher {
            std::size_t operator()(const TextureData& obj) const;
        };

        struct TextureData_Equality_Operator {
            bool operator()(const TextureData& first, const TextureData& second) const;
        };

        struct TextureData_Less_Than_Operator {
            bool operator()(const TextureData& first, const TextureData& second) const;
        };
        
        // Attributes
        SDL_Point destCoords;
    };

    struct PlayerData : public TextureData {};

    struct TeleporterData : public TextureData {
        SDL_Point targetDestCoords;
        std::string targetLevel;
        
        using TeleporterDataCollection = std::unordered_set<TeleporterData, TeleporterData::TextureData_Hasher, TeleporterData::TextureData_Equality_Operator>;
    };


    /**
     * @brief Represents/simulates a level/sub-level.
    */
    struct LevelData {
        tiledata::TileCollection tileCollection;
        SDL_Color backgroundColor;
        leveldata::PlayerData playerLevelData;
        TeleporterData::TeleporterDataCollection teleportersLevelData;
    };
};

/**
 * @brief The global namespace. Member variables accessible to all classes.
*/
namespace globals {
    /**
     * @brief Provide required configuration, used once in `Game` initialization and may extend to other retrieval operations. Should not be modified elsewhere.
    */
    namespace config {
        const InitFlags flags = {
            SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
            SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC,
            IMG_INIT_PNG,
            {
                {SDL_HINT_RENDER_SCALE_QUALITY, "1"},
            }
        };
        const SDL_Rect dims = {
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1280, 720,
        };
        const int frameRate = 120;
        const SDL_Color DEFAULT_BACKGROUND_COLOR = {0x14, 0x14, 0x12, SDL_ALPHA_OPAQUE};
        const std::string DEFAULT_LEVEL = "level-equilibrium";

        const std::filesystem::path ASSETS_PATH = "assets";
        const std::filesystem::path TILED_ASSETS_PATH = ASSETS_PATH / ".tiled";
        const std::filesystem::path TILED_TILESETS_PATH = TILED_ASSETS_PATH / ".tsx";
        const std::filesystem::path PLAYER_TILESET_PATH = TILED_TILESETS_PATH / "hp-player.tsx";
        const std::filesystem::path LEVELS_PATH = TILED_ASSETS_PATH / "levels.json";
        
        const SDL_Point ANIMATED_DYNAMIC_TEXTURE_VELOCITY = {1, 1};
    };

    /**
     * @note The game intends to run on ONE window therefore denecessitates multiple `SDL_Renderer`. This global instance is intended to be accessible by all classes. 
     * @see https://stackoverflow.com/questions/12506979/what-is-the-point-of-an-sdl2-texture
    */
    extern SDL_Renderer* renderer;
    /**
     * @brief The size of the in-game window.
     * @note Its value is assigned after game initialization, then updated whenever the window is resized.
     * @see <src/game.cpp> Game.blit() (classmethod)
    */
    extern SDL_Point windowSize;

    /**
     * @brief The maximum dimension of each tile on the window.
    */
    extern SDL_Point tileDestSize;

    /**
     * @brief The maximum number of tiles per dimension on the window.
    */
    extern SDL_Point tileDestCount;

    /**
     * @brief Intends to "center" the rendereed texture.
    */
    extern SDL_Point windowOffset;

    /**
     * @brief An unordered map that maps `TileSet` with the associated data. Required for many texture-related operations.
     * @note Its value is assigned during initialization and should be treated as a constant henceforth.
    */
    extern tiledata::TilelayerTilesetData::TilelayerTilesetDataCollection tilesetDataCollection;

    /**
     * @brief Represents the current level/sub-level. Accessible to all classes.
    */
    extern leveldata::LevelData currentLevelData;

    void dealloc();
}