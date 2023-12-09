#ifndef GLOBALS_H
#define GLOBALS_H

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>
#include <pugixml/pugixml.hpp>


/**
 * Register JSON for Modern C++ i.e. `nlohmann::json` as the only JSON library in the scope of this project.
*/
using json = nlohmann::json;


/**
 * Contain possible game states. Should be used closely with the main control flow.
*/
enum class GameState {
    kMenu,
    kIngamePlaying,
    kIngamePaused,
    kIngameDialogue,
    kIngameCutscene,
    kExit,
};

/**
 * Contain registered interaction types.
 * @see <interaction.h>
*/
enum class InteractionType {
    kCoords, kRect, kPerPixel,
};

/**
 * @brief Provide flexible handling base on a move's status.
*/
enum class MoveStatusFlag {
    kDefault, kInvalidated, kContinued,
};

/**
 * @brief Contain SDL flags and other configurations used in the initialization of SDL subsystems.
 * 
 * @param init any of the following: `SDL_INIT_TIMER` (timer subsystem), `SDL_INIT_AUDIO` (audio subsystem), `SDL_INIT_VIDEO` (video subsystem - automatically initializes the events subsystem), `SDL_INIT_JOYSTICK` (joystick subsystem - automatically initializes the events subsystem), `SDL_INIT_HAPTIC` (haptic i.e. force feedback subsystem), `SDL_INIT_GAMECONTROLLER` (controller subsystem - automatically initializes the joystick subsystem), `SDL_INIT_EVENTS` (events subsystem), `SDL_INIT_EVERYTHING` (all of the above subsystems), `SDL_INIT_NOPARACHUTE` (compatibility - will be ignored), or multiple OR'd together. Determines which SDL subsystem(s) to initialize. Used in `SDL_Init()`. @see https://wiki.libsdl.org/SDL2/SDL_Init
 * @param window an enumeration constant of `SDL_WindowFlags` or multiple OR'd together. Determintes initial window properties.  @see https://wiki.libsdl.org/SDL2/SDL_WindowFlags
 * @param image an enumeration constant of `IMG_InitFlags` or multiple OR'd together. Determines which image format(s) to be used in the scope of this project. @see https://wiki.libsdl.org/SDL2_image/IMG_Init
 * @param renderer an enumeration constant of `SDL_RendererFlags` or multiple OR'd together. Determines the rendering context. @see https://wiki.libsdl.org/SDL2/SDL_RendererFlags
 * @param hints maps a hint with an associated value. Used in `SDL_SetHint()` and `SDL_SetHintWithPriority()`.
 * 
 * @note Multiple flags are combinable via bitwise OR `|`.
 * @see <game.h> Game::Game()
*/
struct GameFlag {
    Uint32 init;
    Uint32 window;
    Uint32 renderer;
    int image;
    std::unordered_map<std::string, std::string> hints;
};

/**
 * @brief Contain data associated with an entity's main stats i.e. attributes. Govern `EntitySecondaryStats`.
 * @param Vigor attribute governing `HP`.
 * @param Mind attribute governing `FP`.
 * @param Endurance attribute governing `PhysicalDefense`.
 * @param Strength attribute governing `PhysicalAttackDamage`.
 * @param Dexterity attribute governing `Stamina` and `Poise`.
 * @param Intelligence attribute governing `MagicDefense`.
 * @param Faith attribute governing `MagicDamage`.
 * @param Arcane attribute governing `CriticalChance`.
 * @todo Implement proper attribute scaling.
*/
struct EntityPrimaryStats {
    int Vigor;
    int Mind;
    int Endurance;
    int Strength;
    int Dexterity;
    int Intelligence;
    int Faith;
    int Arcane;
};

/**
 * @brief Contain data associated with an entity's secondary stats. Governed by `EntityPrimaryStats`.
 * @param HP Hit Points. Serves as a measure of an entity's health and how much damage it could take before falling in combat. Governed by `Vigor`.
 * @param FP Focus Points. Serves as an energy source consumed when casting Spells or using Skills. Works identically to "MP"/"Mana" in other titles. Governed by `Mind`.
 * @param Stamina a resource consumed when sprinting, attacking, or casting spells. Governed by `Dexterity`.
 * @param Poise the degree to which an entity could resist collapsing under incoming attacks. Governed by `Dexterity`.
 * @param PhysicalDefense representative of an entity's defense power and damage negation against physical attacks. Governed by `Endurance`. In percent.
 * @param MagicDefense representative of an entity's defense power and damage negation against magical attacks. Governed by `Intelligence`. In percent.
 * @param PhysicalDamage representative of the amount of damage dealt in by physical attacks. Governed by `Strength`.
 * @param MagicDamage representative of the amount of damage dealt by magical attacks. Governed by `Faith`.
 * @param CriticalChance representative of the entity's chance of landing a critical attack. Governed by `Arcane`. In percent.
 * @param CriticalDamage a multiplier; representative of the amount of damage increased by critical attack compared to the "normal" counterparts. In percent.
*/
struct EntitySecondaryStats {
    int HP;
    int FP;
    int Stamina;
    int Poise;

    double PhysicalDefense;
    double MagicDefense;
    int PhysicalDamage;
    int MagicDamage;

    double CriticalChance;
    double CriticalDamage;

    void initialize(const EntityPrimaryStats& entityPrimaryStats);
    static int calculateFinalizedPhysicalDamage(EntitySecondaryStats& active, EntitySecondaryStats& passive);
    static int calculateFinalizedMagicDamage(EntitySecondaryStats& active, EntitySecondaryStats& passive);
};


/**
 * @brief Group components that are associated with tiles.
 * @see <utils.h> utils::loadTilesetsData()
 * @see <interface.h> IngameInterface
*/
namespace tile {
    /**
     * The base unit used for manipulation and retrieval of a level's data. Contains `GID`s. ("Global Tile IDs" in Tiled terminology, essentially non-negative integers)
     * @see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data
    */
    using Tile = std::vector<int>;

    /**
     * A 2-dimensional iterable of `Tile`. Represents a 2-dimensional tiled surface.
     * @todo Separate into layers for dynamic manipulation mid-runtime.
    */
    using TileCollection = std::vector<std::vector<Tile>>;

    /**
     * @brief Contains data associated with the tilelayer used for rendering.
     * 
     * @param textures each element determines the tileset to which the `Tile` belongs, per tilelayer.
     * @param srcRects has the same size as `textures`. `srcRects[i]` corresponds to `textures[i]` and is used in conjunction to determine the specific portion of the tileset used for rendering.
     * @param destRect the specific portion of the window on which the `Tile` is rendered. Independent of the tileset, its value is determined only by `globals::tileDestSize` and `globals::windowOffset`.
     * 
     * @note Not to be confused with `TilelayerTilesetData`.
     * @note Recommended implementation: instances should only be instantiated immediately before rendering and should be destantiated immediately afterwards.
    */
    struct TilelayerRenderData {
        /**
         * A 2-dimensional iterable of `TileRenderData`. `TilelayerRenderData::Collection[i][j]` corresponds to `TileCollection[i][j]` and is used in conjunction to determine the rendering data.
        */
        using Collection = std::vector<std::vector<TilelayerRenderData>>;

        std::vector<SDL_Texture*> textures;
        std::vector<SDL_Rect> srcRects;
        SDL_Rect destRect;
    };

    /**
     * @brief Contain data associated with a generic tileset.
     * 
     * @param texture represents the tileset.
     * @param srcCount the number of `Tile` per dimension in the tileset.
     * @param srcSize the maximum dimension of a `Tile` in the tileset.
     * @param properties maps the tileset's properties to their stringified values. Properties are Tiled standard types only e.g., `string`, `bool`, `int`. Registered values: `"norender"` prevents the tileset from being rendered; `"collision"` enables the tileset to be used in collision detection.
     * 
     * @note `initialize()` method of this and derived classes are implemented in elsewhere - `<utils.h>`'s source file.
    */
    struct BaseTilesetData {
        BaseTilesetData();
        void initialize(pugi::xml_document& document, SDL_Renderer* renderer);
        void deinitialize();

        SDL_Texture* texture;
        SDL_Point srcCount;
        SDL_Point srcSize;
        std::unordered_map<std::string, std::string> properties;
    };

    /**
     * @brief Contain data associated with a tilelayer's tileset.
     * 
     * @param firstGID represents the first tile in the tileset. Regulated by the level configuration file. Should be treated as a constant as manipulation could lead to undefined behaviors.
     * @see <globals.h> tile::BaseTilesetData
    */
    struct TilelayerTilesetData : public BaseTilesetData {
        /**
         * An ordered iterable of `TilelayerTilesetData`, sorted by `firstGID`.
         * @note Recommended implementation: instances should be (re)initialized once per `IngameInterface::loadLevel()` call, should be treated as a constant otherwise, its lifespan should not exceed beyond the scope of the aforementioned classmethod.
         * @see <interface.h> IngameInterface::loadLevel()
        */
        using Collection = std::vector<TilelayerTilesetData>;

        TilelayerTilesetData();
        void initialize(const json& tileset, SDL_Renderer* renderer);

        int firstGID;
    };

    /**
     * @brief Contain data associated with a tileset for an entity or an animated object.
     * 
     * @param animationMapping maps an animation type to the associated data.
     * @param animationUpdateRate the number of frames a sprite should last before switching to the next. Should be treated as a constant.
     * @param animationSize represents the ratio between the size of one single animation/sprite and the size of a `Tile` on the tileset, per dimension. Should be implemented alongside `globals::tileDestSize`.
    */
    struct AnimatedEntitiesTilesetData : public BaseTilesetData {
        /**
         * Register animation types as enumeration constants for maintainability.
        */
        enum class AnimationType {
            kIdle,
            kAttack,
            kBlink,
            kDeath,
            kDisappear,
            kDuck,
            kJump,
            kRunning,
            kWalk,
            kDamaged,
        };

        /**
         * Convert raw `std::string` (from configuration files) into `AnimationType`.
        */
        static const std::unordered_map<std::string, AnimationType> kAnimationTypeConversionMapping;

        /**
         * @brief Contain data associated with an animation i.e. a series of sprites.
         * 
         * @param startGID the first `GID` of the animation. Defaults to `0`.
         * @param stopGID the last `GID` of the animation. Defaults to `0`.
         * @param isPermanent specifies whether the animation should be a loop i.e. whether external calls should be performed when the animation reaches its end. Defaults to `false`.
        */
        struct Animation {
            int startGID = 0;
            int stopGID = 0;
            bool isPermanent = false;
        };

        void initialize(pugi::xml_document& document, SDL_Renderer* renderer);

        std::unordered_map<AnimationType, Animation> animationMapping;
        int animationUpdateRate = 64;
        SDL_Point animationSize = {1, 1};
    };

    /**
     * @brief Contain data associated with the pending i.e. "next" animation.
    */
    struct NextAnimationData {
        bool isExecuting = false;
        AnimatedEntitiesTilesetData::AnimationType animationType;

        NextAnimationData(AnimatedEntitiesTilesetData::AnimationType animationType);
        static void update(NextAnimationData*& instance, const tile::AnimatedEntitiesTilesetData::AnimationType pendingAnimationType);
    };
};

/**
 * @brief Group components that are associated to level and level-loading.
*/
namespace level {
    /**
     * Register level names as enumeration constants for maintainability.
    */
    enum class LevelName {
        kLevelEquilibrium,
        kLevelValleyOfDespair,
    };

    /**
     * Convert raw `std::string` (from configuration files) into `LevelName`.
    */
    extern const std::unordered_map<std::string, LevelName> kLevelNameConversionMapping;

    /**
     * Map a level name to the corresponding relative file path.
     * @note Recommended implementation: instances should only be initialized once during `<interface.h> IngameInterface::initialize()` and should henceforth be treated as a constant. Additionally, `Collection` must be re-declared in every derived class declaration.
     * @note For optimized memory usage, this approach does not encapsulate `LevelData`, instead `globals::currentLevelData` is loaded via `<interface.h> IngameInterface::loadLevel()` based on file path.
    */
    using LevelMapping = std::unordered_map<LevelName, std::string>;

    /**
     * @brief Contain data associated with an entity, used in level-loading.
     * @param destCoords the new `destCoords` of the entity upon entering new level.
    */
    struct EntityLevelData {
        struct Hasher {
            std::size_t operator()(const EntityLevelData& obj) const;
        };

        struct Equality_Operator {
            bool operator()(const EntityLevelData& first, const EntityLevelData& second) const;
        };

        struct Less_Than_Operator {
            bool operator()(const EntityLevelData& first, const EntityLevelData& second) const;
        };
        
        /**
         * Denecessitate duplicated declaration of `EntityLevelData::Collection` per derived class.
        */
        template <typename T>
        using Collection = std::unordered_set<T, EntityLevelData::Hasher, EntityLevelData::Equality_Operator>;

        virtual void initialize(const json& entityJSONLevelData);
        virtual ~EntityLevelData() = default;   // Virtual destructor, required for polymorphism
        
        SDL_Point destCoords;
    };


    /**
     * @brief Contain data associated with a player entity, used in level-loading.
    */
    struct PlayerLevelData : public EntityLevelData {};

    /**
     * @brief Contain data associated with a teleporter entity, used in level-loading.
     * @param targetDestCoords the new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
     * @param targetLevel the new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
    */
    struct TeleporterLevelData : public EntityLevelData {
        void initialize(const json& entityJSONLevelData) override;

        SDL_Point targetDestCoords;
        level::LevelName targetLevel;
    };

    /**
     * @brief Contain data associated with a slime entity, used in level-loading.
    */
    struct SlimeLevelData : public EntityLevelData {};

    /**
     * @brief Contain data associated with a level.
     * 
     * @param tileCollection the 2-dimensional tiled surface.
     * @param backgroundColor the background color.
     * @param playerLevelData data associated with the player entity.
     * @param teleportersLevelData data associated with the teleporter entities.
    */
    struct LevelData {
        tile::TileCollection tileCollection;
        SDL_Color backgroundColor;
        level::PlayerLevelData playerLevelData;
        level::EntityLevelData::Collection<TeleporterLevelData> teleportersLevelData;
        level::EntityLevelData::Collection<SlimeLevelData> slimesLevelData;

        void initialize(const json& JSONLayerData);
        void deinitialize();
    };
};

/**
 * @brief Group components that are accessible at public scope to all other components.
*/
namespace globals {
    /**
     * @brief Group components that are used for configuration.
     * @note Recommended implementation: components should be retrieved once in `Game` initialization and may extend to other retrieval operations, manipulation is not possible.
    */
    namespace config {
        const GameFlag kGameFlags = {
            SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
            SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC,
            IMG_INIT_PNG,
            {
                {SDL_HINT_RENDER_SCALE_QUALITY, "1"},
            }
        };
        constexpr SDL_Rect kWindowDimension = {
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1280, 720,
        };
        constexpr int kFrameRate = 120;
        constexpr SDL_Color kDefaultBackgroundColor = {0x14, 0x14, 0x12, SDL_ALPHA_OPAQUE};
        constexpr level::LevelName kDefaultLevelName = level::LevelName::kLevelEquilibrium;

        const std::filesystem::path kAssetPath = "assets";
        const std::filesystem::path kTiledAssetPath = kAssetPath / ".tiled";
        const std::filesystem::path kTilesetPath = kTiledAssetPath / ".tsx";
        const std::filesystem::path kTilesetPathPlayer = kTilesetPath / "hp-player.tsx";
        const std::filesystem::path kTilesetPathTeleporter = kTilesetPath / "mi-a-cat.tsx";
        const std::filesystem::path kTilesetPathSlime = kTilesetPath / "eg-slime-full.tsx";
        const std::filesystem::path kConfigPathLevel = kTiledAssetPath / "levels.json";
        
        constexpr double kDefaultEntityRunVelocityModifier = 2;
        constexpr SDL_FRect kDefaultEntityDestRectModifier = {0, 0, 1, 1};
        constexpr SDL_FRect kDefaultPlayerDestRectModifier = {0, -0.75, 2, 2};
        constexpr SDL_FRect kDefaultTeleporterDestRectModifier = {0, 0, 1, 1};
        constexpr SDL_FRect kDefaultSlimeDestRectModifier = {0, -1, 5, 5};

        constexpr SDL_FPoint kDefaultPlayerVelocity = {32, 32};
        constexpr SDL_FPoint kDefaultSlimeVelocity = {128, 128};

        constexpr int kDefaultPlayerMoveDelay = 0;
        constexpr int kDefaultSlimeMoveDelay = 32;

        constexpr SDL_Point kDefaultSlimeMoveInitiateRange = {16, 16};
        constexpr SDL_Point kDefaultPlayerAttackRegisterRange = {99, 99};
        constexpr SDL_Point kDefaultSlimeAttackInitiateRange = {7, 7};
        constexpr SDL_Point kDefaultSlimeAttackRegisterRange = {5, 5};

        constexpr EntityPrimaryStats kDefaultPlayerPrimaryStats = {10, 10, 10, 10, 10, 10, 10, 10};
        constexpr EntityPrimaryStats kDefaultSlimePrimaryStats = {10, 10, 10, 10, 10, 10, 10, 10};
    };

    void deinitialize();

    /**
     * The global `SDL_Renderer`.
     * @note Recommended implementation: this instance should be used as the only `SDL_Renderer` in the scope of this project. Explanation: this project only intends to run on one window.
     * @see https://stackoverflow.com/questions/12506979/what-is-the-point-of-an-sdl2-texture
    */
    extern SDL_Renderer* renderer;
    /**
     * Represents the width and height of the in-game window.
     * @note Recommended implementation: this instance should be assigned during `<game.h> Game::initialize()`, then reassigned in `<game.h> Game::onWindowChange().
    */
    extern SDL_Point windowSize;

    /**
     * The maximum dimension of each `Tile` on the window, in pixels.
    */
    extern SDL_Point tileDestSize;

    /**
     * The maximum number of `Tile` on the window, per dimension.
    */
    extern SDL_Point tileDestCount;

    /**
     * The modifier for `destRect` used in rendering. Aims to center the rendered textures.
    */
    extern SDL_Point windowOffset;

    /**
     * Store data associated with tilelayer tilesets of the current level.
    */
    extern tile::TilelayerTilesetData::Collection tilelayerTilesetDataCollection;

    /**
     * Contain data associated with the current level.
    */
    extern level::LevelData currentLevelData;
};


#endif