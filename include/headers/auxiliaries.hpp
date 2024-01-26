#ifndef AUXILIARIES_H
#define AUXILIARIES_H

#include <array>
#include <filesystem>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <nlohmann/json.hpp>
#include <pugixml/pugixml.hpp>


/**
 * Register JSON for Modern C++ i.e. `nlohmann::json` as the only JSON library in the scope of this project.
*/
using json = nlohmann::json;


/* Enumerations & Structs */

/**
 * Contain possible game states. Should be used closely with the main control flow.
*/
enum class GameState {
    kExit = 1,
    kMenu = 2,
    kLoading = 4,
    kIngamePlaying = 8,
    kGameOver = 16,
    // kIngamePaused,
    // kIngameDialogue,
    // kIngameCutscene,
    k6 = 6,
    k12 = 12,   // Prevent warnings
};

constexpr GameState operator|(GameState const& first, GameState const& second) {
    return static_cast<GameState>(static_cast<int>(first) | static_cast<int>(second));   // Must be defined here
}

/**
 * Contain possible in-game view modes.
*/
enum class IngameViewMode {
    kFullScreen = 1,
    kFocusOnEntity = 2,
};

/**
 * Contain registered interaction types.
 * @see <interaction.h>
*/
enum class InteractionType {
    kCoordsArb, kCoordsAbs, kRect, kPerPixel,
};

/**
 * @brief Provide flexible handling base on a move's status.
*/
enum class EntityStatusFlag {
    kDefault, kInvalidated, kContinued,
};

/**
 * @brief Contain SDL flags and other configurations used in the initialization of SDL subsystems.
 * 
 * @param lSDL any of the following: `SDL_INIT_TIMER` (timer subsystem), `SDL_INIT_AUDIO` (audio subsystem), `SDL_INIT_VIDEO` (video subsystem - automatically initializes the events subsystem), `SDL_INIT_JOYSTICK` (joystick subsystem - automatically initializes the events subsystem), `SDL_INIT_HAPTIC` (haptic i.e. force feedback subsystem), `SDL_INIT_GAMECONTROLLER` (controller subsystem - automatically initializes the joystick subsystem), `SDL_INIT_EVENTS` (events subsystem), `SDL_INIT_EVERYTHING` (all of the above subsystems), `SDL_INIT_NOPARACHUTE` (compatibility - will be ignored), or multiple OR'd together. Determines which SDL subsystem(s) to initialize. Used in `SDL_Init()`. @see https://wiki.libsdl.org/SDL2/SDL_Init
 * @param lIMG an enumeration constant of `IMG_InitFlags` or multiple OR'd together. Determines which image format(s) to be used in the scope of this project. @see https://wiki.libsdl.org/SDL2_image/IMG_Init
 * @param window an enumeration constant of `SDL_WindowFlags` or multiple OR'd together. Determintes initial window properties.  @see https://wiki.libsdl.org/SDL2/SDL_WindowFlags
 * @param renderer an enumeration constant of `SDL_RendererFlags` or multiple OR'd together. Determines the rendering context. @see https://wiki.libsdl.org/SDL2/SDL_RendererFlags
 * @param hints maps a hint with an associated value. Used in `SDL_SetHint()` and `SDL_SetHintWithPriority()`.
 * 
 * @note Multiple flags are combinable via bitwise OR `|`.
 * @see <game.h> Game::Game()
*/
struct GameInitFlag {
    Uint32 lSDL;
    int lIMG;

    Uint32 window;
    Uint32 renderer;
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

    void initialize(EntityPrimaryStats const& entityPrimaryStats);
    static int calculateFinalizedPhysicalDamage(EntitySecondaryStats const& active, EntitySecondaryStats const& passive);
    static int calculateFinalizedMagicDamage(EntitySecondaryStats const& active, EntitySecondaryStats const& passive);
};

/**
 * @brief Contain data associated with a text area's configurations.
*/
struct ComponentPreset {
    SDL_Color backgroundColor;
    SDL_Color lineColor;
    SDL_Color textColor;

    float lineOffset;
    float lineWidth;
};

/**
 * @brief Group components that are associated with tiles.
 * @see <utils.h> utils::loadTilesetsData()
 * @see <interface.h> IngameMapHandler
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
         * @note Recommended implementation: instances should be (re)initialized once per `IngameMapHandler::loadLevel()` call, should be treated as a constant otherwise, its lifespan should not exceed beyond the scope of the aforementioned classmethod.
         * @see <interface.h> IngameMapHandler::loadLevel()
        */
        using Collection = std::vector<TilelayerTilesetData>;

        void initialize(json const& tileset, SDL_Renderer* renderer);

        int firstGID = 0;
    };

    /**
     * @brief Contain data associated with a tileset for an entity or an animated object.
     * 
     * @param animationMapping maps an animation type to the associated data.
     * @param animationUpdateRate the number of frames a sprite should last before switching to the next. Should be treated as a constant.
     * @param animationSize represents the ratio between the size of one single animation/sprite and the size of a `Tile` on the tileset, per dimension. Should be implemented alongside `globals::tileDestSize`.
    */
    struct EntitiesTilesetData : public BaseTilesetData {
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
            kRun,
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
            double animationUpdateRateMultiplier = 1;
        };

        void initialize(pugi::xml_document& document, SDL_Renderer* renderer);

        std::unordered_map<AnimationType, Animation> animationMapping;
        int animationUpdateRate = 64;
        SDL_Point animationSize = {1, 1};
    };
}


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
     * @note Recommended implementation: instances should only be initialized once during `<interface.h> IngameMapHandler::initialize()` and should henceforth be treated as a constant. Additionally, `Collection` must be re-declared in every derived class declaration.
     * @note For optimized memory usage, this approach does not encapsulate `LevelData`, instead `globals::currentLevelData` is loaded via `<interface.h> IngameMapHandler::loadLevel()` based on file path.
    */
    using LevelMapping = std::unordered_map<LevelName, std::string>;

    /**
     * @brief Contain data associated with an entity, used in level-loading.
     * @param destCoords the new `destCoords` of the entity upon entering new level.
    */
    struct EntityLevelData {
        struct hash {
            std::size_t operator()(EntityLevelData const& instance) const;
        };

        struct equal_to {
            bool operator()(EntityLevelData const& first, EntityLevelData const& second) const;
        };

        /**
         * Denecessitate duplicated declaration of `EntityLevelData::Collection` per derived class.
        */
        template <typename T>
        using Collection = std::unordered_set<T, hash, equal_to>;

        virtual void initialize(json const& entityJSONLevelData);
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
        void initialize(json const& entityJSONLevelData) override;

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
        level::EntityLevelData::Collection<level::TeleporterLevelData> teleportersLevelData;
        level::EntityLevelData::Collection<level::SlimeLevelData> slimesLevelData;

        void initialize(json const& JSONLayerData);
        void deinitialize();
    };
}


using AnimationType = tile::EntitiesTilesetData::AnimationType;


/**
 * @brief Group components that are used for configuration.
 * @note Recommended implementation: components should be retrieved once in `Game` initialization and may extend to other retrieval operations, manipulation is not possible.
 * @note All namespace members, even without Google's recommended `"k"` prefix, are `const`/`constexpr`.
*/
namespace config {
    namespace path {
        const std::filesystem::path asset = "assets";
        const std::filesystem::path asset_tiled = asset / ".tiled";
        const std::filesystem::path asset_font = asset / "fonts";
        const std::filesystem::path asset_audio = asset / "audio";
        
        namespace font {
            const std::filesystem::path OmoriChaotic = asset_font / "omori-game-1.ttf";
            const std::filesystem::path OmoriHarmonic = asset_font / "omori-game-2.ttf";
            const std::filesystem::path DeterminationMono = asset_font / "DeterminationMonoWebRegular-Z5oq.ttf";
            const std::filesystem::path BadlyStuffedAnimal = asset_font / "BadlyStuffedAnimalDemoReg-8M3AD.ttf";   // Bizarre
            const std::filesystem::path BadComa = asset_font / "bad-coma.ttf";
        }
    }

    namespace key {
        constexpr SDL_Keycode EXIT = SDLK_ESCAPE;

        constexpr SDL_Keycode INGAME_LEVEL_RESET = SDLK_F4;
        constexpr SDL_Keycode INGAME_TOGGLE_CAMERA_ANGLE = SDLK_F5;
        constexpr SDL_Keycode INGAME_TOGGLE_GRAYSCALE = SDLK_F6;
        constexpr SDL_Keycode INGAME_RETURN_MENU = SDLK_F1;

        constexpr SDL_Keycode PLAYER_MOVE_UP = SDLK_w;
        constexpr SDL_Keycode PLAYER_MOVE_DOWN = SDLK_s;
        constexpr SDL_Keycode PLAYER_MOVE_RIGHT = SDLK_a;
        constexpr SDL_Keycode PLAYER_MOVE_LEFT = SDLK_d;
        constexpr SDL_Keycode PLAYER_RUN_TOGGLE = SDLK_LSHIFT;
        constexpr SDL_KeyCode PLAYER_ATTACK = SDLK_SPACE;
        constexpr SDL_KeyCode PLAYER_LINEAR_SURGE_ATTACK = SDLK_q;
    }

    namespace color {
        constexpr SDL_Color offwhite = SDL_Color{ 0xf2, 0xf3, 0xf4, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color offblack = { 0x14, 0x14, 0x12, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color black = SDL_Color{ 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color smoky_black = SDL_Color{ 0x10, 0x0c, 0x08, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color gold = SDL_Color{ 0xff, 0xd7, 0x00, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color transparent = SDL_Color{ 0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT };
    }

    namespace preset {
        constexpr ComponentPreset lightButton = {
            config::color::offwhite, config::color::black, config::color::black, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset darkButton = {
            config::color::black, config::color::offwhite, config::color::offwhite, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset yellowButton = {
            config::color::black, config::color::gold, config::color::gold, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset frameRateOverlay = {
            config::color::smoky_black, config::color::transparent, config::color::offwhite, 0, 0,
        };
        constexpr ComponentPreset title = {
            config::color::transparent, config::color::transparent, config::color::offwhite, 0, 0,
        };
        constexpr ComponentPreset exitText = title;
        constexpr ComponentPreset loadingMessage = title;
    }

    namespace game {
        constexpr int frameRate = 60;
        const std::tuple<GameInitFlag, SDL_Rect, int, std::string> initializer = {
            {
                SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS,
                IMG_INIT_PNG,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
                SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC,
                {
                    { SDL_HINT_RENDER_SCALE_QUALITY, "1" },
                }
            },
            { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720 },
            frameRate,
            "8964",
        };
    }

    namespace mixer {
        // Different initialization process
        constexpr int frequency = MIX_DEFAULT_FREQUENCY;
        constexpr uint16_t format = MIX_DEFAULT_FORMAT;
        constexpr int channels = MIX_DEFAULT_CHANNELS;
        constexpr int chunkSize = 2048;   // 2 KB
    }

    namespace interface {
        const std::filesystem::path path = "assets/.tiled/levels.json";
        constexpr level::LevelName levelName = level::LevelName::kLevelEquilibrium;
        constexpr int idleFrames = 16;

        constexpr IngameViewMode defaultViewMode = IngameViewMode::kFocusOnEntity;
        constexpr double tileCountHeight = 13;
        constexpr double grayscaleIntensity = 0.5;
    }

    namespace entities {
        constexpr double runVelocityModifier = 2;
        constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
        
        namespace player {
            const std::filesystem::path path = "assets/.tiled/.tsx/hp-player.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -0.75f, 2, 2 };
            constexpr SDL_FPoint velocity = { 32, 32 };
            constexpr int moveDelay = 0;
            constexpr SDL_Point attackRegisterRange = { 99, 99 };
            constexpr EntityPrimaryStats primaryStats = { 10, 10, 10, 10, 10, 10, 10, 10 };
        }

        namespace teleporter {
            const std::filesystem::path path = "assets/.tiled/.tsx/mi-a-cat.tsx";
            constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
            constexpr EntityPrimaryStats primaryStats = { 0, 0, 0, 0, 0, 0, 0, 0 };
        }

        namespace slime {
            const std::filesystem::path path = "assets/.tiled/.tsx/eg-slime-full.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -1, 5, 5 };
            constexpr SDL_FPoint velocity = { 128, 128 };
            constexpr int moveDelay = 32;
            constexpr SDL_Point moveInitiateRange = { 16, 16 };
            constexpr SDL_Point attackInitiateRange = { 7, 7 };
            constexpr SDL_Point attackRegisterRange = { 5, 5 };
            constexpr EntityPrimaryStats primaryStats = { 20, 10, 0, 10, 10, 0, 0, 0 };
        }

        namespace pentacle_projectile {
            const std::filesystem::path path = "assets/.tiled/.tsx/mi-a-pentacle.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -1, 1, 1 };
            constexpr SDL_FPoint velocity = { 0, 0 };
            constexpr int moveDelay = 0;
            constexpr SDL_Point attackRegisterRange = { 1, 1 };
            constexpr EntityPrimaryStats primaryStats = { 0, 0, 0, 0, 0, 0, 10, 0 };
        }

        namespace haunted_bookcase_projectile {
            const std::filesystem::path path = "assets/.tiled/.tsx/mi-a-haunted-bookcase.tsx";
            constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
            constexpr SDL_FPoint velocity = { 16, 16 };
            constexpr int moveDelay = 0;
            constexpr SDL_Point attackRegisterRange = { 1, 1 };
            constexpr EntityPrimaryStats primaryStats = { 0, 0, 0, 0, 0, 0, 10, 0 };
        }
    }

    namespace components {
        constexpr SDL_Point destRectRatio = { 10, 2 };

        namespace fps_overlay {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.1f, 0.1f }, config::preset::frameRateOverlay, "");
            constexpr double destSizeModifier = 0.25;
            constexpr SDL_Point destRectRatio = { 5, 2 };
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
            constexpr int updateRate = 30;

            constexpr unsigned int precision = 2;
            const std::string prefix = "FPS: ";
        }

        namespace exit_text {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.1f, 0.05f }, config::preset::exitText, "quitting~");
            constexpr double destSizeModifier = 0.5;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::DeterminationMono;
            constexpr double progressUpdateRateLimit = 1;
            constexpr double progressUpdateRate = progressUpdateRateLimit / static_cast<double>(config::game::frameRate >> 2);
        }

        namespace menu_avatar {
            constexpr double destRectModifier = 0.125;
        }

        namespace menu_animated_background {
            constexpr double animationUpdateRate = 0.001;
        }

        namespace menu_button {
            const std::array<std::tuple<SDL_FPoint, ComponentPreset, ComponentPreset, std::string, GameState*>, 4> initializer = {
                std::make_tuple(SDL_FPoint{ 1.0f / 3.0f, 7.0f / 9.0f }, config::preset::lightButton, config::preset::darkButton, "NEW GAME", new GameState(GameState::kLoading | GameState::kIngamePlaying)),
                std::make_tuple(SDL_FPoint{ 1.0f / 3.0f, 8.0f / 9.0f }, config::preset::lightButton, config::preset::darkButton, "CONTINUE", nullptr),
                std::make_tuple(SDL_FPoint{ 2.0f / 3.0f, 7.0f / 9.0f }, config::preset::lightButton, config::preset::darkButton, "SETTINGS", nullptr),
                std::make_tuple(SDL_FPoint{ 2.0f / 3.0f, 8.0f / 9.0f }, config::preset::lightButton, config::preset::darkButton, "ABOUT", nullptr),
            };
            constexpr double destSizeModifier = 1;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
        }

        namespace menu_title {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 0.2f }, config::preset::title, "8964");
            constexpr double destSizeModifier = 5.5;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::BadComa;
        }

        namespace loading_message {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 4.0f / 9.0f }, config::preset::loadingMessage, "loading");
            constexpr double destSizeModifier = 2;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
        }

        namespace loading_progress_bar {
            const std::tuple<SDL_FPoint, ComponentPreset> intializer = std::make_tuple(SDL_FPoint{ 0.5f, 5.0f / 9.0f }, config::preset::lightButton);
            constexpr double destSizeModifier = 1;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            constexpr double progressUpdateRateLimit = 1;
            constexpr double progressUpdateRate = 0.02;
        }

        namespace game_over_title {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 0.4f }, config::preset::title, "GAME OVER ...?");
            constexpr double destSizeModifier = 5;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
        }

        namespace game_over_button {
            const std::tuple<SDL_FPoint, ComponentPreset, ComponentPreset, std::string, GameState*> initializer = {
                std::make_tuple(SDL_FPoint{ 0.5f, 0.6f }, config::preset::lightButton, config::preset::yellowButton, "amend", new GameState(GameState::kLoading | GameState::kMenu)),
            };
            constexpr double destSizeModifier = 1;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
        }
    }
}


/**
 * @brief Group components that are accessible at public scope to all other components.
*/
namespace globals {
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
     * The current position of the mouse relative to the window.
    */
    extern SDL_Point mouseState;

    /**
     * Store data associated with tilelayer tilesets of the current level.
    */
    extern tile::TilelayerTilesetData::Collection tilelayerTilesetDataCollection;

    /**
     * Contain data associated with the current level.
    */
    extern level::LevelData currentLevelData;

    /**
     * The current game state. Is strictly bound to the main control flow.
    */
    extern GameState state;
}


bool operator==(SDL_Point const& first, SDL_Point const& second);
bool operator!=(SDL_Point const& first, SDL_Point const& second);
bool operator<(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator+(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator-(SDL_Point const& first, SDL_Point const& second);
bool operator==(SDL_FPoint const& first, SDL_FPoint const& second);

namespace std {
    template <>
    struct hash<SDL_Point> {
        std::size_t operator()(SDL_Point const& instance) const;
    };

    template <>
    struct hash<SDL_FPoint> {
        std::size_t operator()(SDL_FPoint const& instance) const;
    };
}


namespace utils {
    // template <typename Base, typename Derived>
    // struct isDerivedFrom {
    //     static_assert(std::is_base_of_v<Base, Derived>, "`Derived` must derive from `Base`");
    // };

    template <typename Iterable, typename Callable, typename... Args>
    void iterate(Iterable const& iterable, Callable&& callable, Args&&... args) {
        for (const auto& element : iterable) std::invoke(std::forward<Callable>(callable), element, std::forward<Args>(args)...);
    }

    // /**
    //  * @see https://stackoverflow.com/questions/4325154/delete-objects-of-incomplete-type
    // */
    // template <typename T>
    // inline void dealloc(T*& instance) {
    //     // typedef char __type__[sizeof(T) ? 1 : -1]; (void) sizeof(__type__);
    //     if (instance == nullptr) return;
    //     delete instance;
    //     instance = nullptr;
    // }

    // template <typename T, typename Callable, typename... Args>
    // inline void dealloc(T*& instance, Callable&& callable, Args&&... args) {
    //     // typedef char __type__[sizeof(T) ? 1 : -1]; (void) sizeof(__type__);
    //     if (instance == nullptr) return;
    //     std::invoke(std::forward<Callable>(callable), std::forward<Args>(args)...);
    //     instance = nullptr;
    // }

    // template <typename T, typename U, typename Callable, typename... Args>
    // inline void dealloc(T*& instance, U& src, Callable&& callable, Args&&... args) {
    //     // typedef char __type__[sizeof(T) ? 1 : -1]; (void) sizeof(__type__);
    //     if (instance == nullptr) return;
    //     std::invoke(std::forward<Callable>(callable), src, std::forward<Args>(args)...);
    //     instance = nullptr;
    // }
    
    int castFloatToInt(const float f);
    std::string castDoubleToString(const double d, unsigned int precision);
    int generateRandomBinary(const double probability = 0.5);
    double calculateDistance(SDL_Point const& first, SDL_Point const& second);
    SDL_Color SDL_ColorFromHexString(std::string const& hexString);
    void setRendererDrawColor(SDL_Renderer* renderer, SDL_Color const& color);
    SDL_Texture* createGrayscaleTexture(SDL_Renderer* renderer, SDL_Texture* texture, double intensity = 1);

    template <typename T>
    std::vector<T> zlibDecompress(std::string const& s);
    std::string base64Decode(std::string const& s);

    void readJSON(std::filesystem::path const& path, json& data);
    void cleanRelativePath(std::filesystem::path& path);

    void loadLevelsData(level::LevelMapping& mapping);
    void loadLevelData(level::LevelData& currentLevelData, json const& JSONLevelData);
    void loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, json const& jsonData);

    tile::TilelayerTilesetData const* getTilesetData(tile::TilelayerTilesetData::Collection const& tilesetDataCollection, int gid);

    void setTextureRGB(SDL_Texture*& texture, SDL_Color const& color);
    void setTextureRGBA(SDL_Texture*& texture, SDL_Color const& color);  
}


#endif