#ifndef AUXILIARIES_H
#define AUXILIARIES_H

#include <iostream>

#include <array>
#include <cmath>
#include <cstdint>
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

enum class GameState : unsigned short int {
    kExit = 1,
    kMenu = 2,
    kLoading = 4,
    kIngamePlaying = 8,
    kIngameDialogue = 16,
    kGameOver = 32,
    // kIngamePaused,
    // kIngameDialogue,
    // kIngameCutscene,
    __6__ = 6,
    __12__ = 12,   // Prevent warnings
};

constexpr GameState operator|(GameState const& first, GameState const& second) {
    return static_cast<GameState>(static_cast<int>(first) | static_cast<int>(second));   // Must be defined here
}

/**
 * @brief Provide flexible handling base on an entity movement's status.
*/
enum class EntityStatus : unsigned char {
    kDefault,
    kInvalidated,
    kContinued,
};

enum class ProjectileType : unsigned char {
    kOrthogonalSingle,
    kOrthogonalDouble,
    kOrthogonalTriple,
    kOrthogonalQuadruple,
    kDiagonalQuadruple,
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
    static void resolve(EntitySecondaryStats const& active, EntitySecondaryStats& passive);
};

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
     * "Global Tile IDs" in Tiled terminology.
     * @see https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data
    */
    using GID = int;
    
    /**
     * Imagine a level comprised of `Z` layers, which are essentially 2D `X x Y` arrays of `GID`. The combination of coordinate `x,y` from each layer, in order, forms an `Z`-sized array of `GID`. We call that array `Slice`.
    */
    using Slice = std::vector<GID>;

    /**
     * Imagine `X x Y` `Z`-sized `Slice` placed next to each other, in order, forming a `X x Y x Z` 3D array of `GID`. We call that `Tensor`.
     * @todo Separate into layers for dynamic manipulation mid-runtime.
    */
    using Tensor = std::vector<std::vector<Slice>>;

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
    struct Data_Tile_RenderOnly {
        /**
         * A 2-dimensional iterable of `TileRenderData`. `Data_Tile_RenderOnly::Collection[i][j]` corresponds to `Tensor[i][j]` and is used in conjunction to determine the rendering data.
        */
        using Collection = std::vector<std::vector<Data_Tile_RenderOnly>>;

        std::vector<SDL_Texture*> textures;
        std::vector<SDL_Rect> srcRects;
        SDL_Rect destRect;
    };

    /**
     * @brief Contain data associated with a generic tileset.
     * @param properties maps the tileset's properties to their stringified values. Properties are Tiled standard types only e.g., `string`, `bool`, `int`. Registered values: `"norender"` prevents the tileset from being rendered; `"collision"` enables the tileset to be used in collision detection.
    */
    struct Data_Generic {
        std::string getProperty(std::string const& key);
        void setProperty(std::string const& key, std::string const& property);

        void load(pugi::xml_document const& XMLTilesetData, SDL_Renderer* renderer);
        void clear();

        SDL_Texture* texture;
        SDL_Point srcCount;
        SDL_Point srcSize;
        std::unordered_map<std::string, std::string> properties;
    };

    /**
     * @brief Contain data associated with a tilelayer's tileset.
     * @param firstGID represents the first tile in the tileset. Regulated by the level configuration file. Should be treated as a constant as manipulation could lead to undefined behaviors.
     * @see <globals.h> tile::BaseTilesetData
    */
    struct Data_TilelayerTileset : public Data_Generic {
        void load(json const& JSONTileLayerData, SDL_Renderer* renderer);   // Does not override

        GID firstGID = 0;
    };

    /**
     * An ordered iterable of `TilelayerTilesetData`, sorted by `firstGID`.
     * @note Recommended implementation: instances should be (re)initialized once per `IngameMapHandler::loadLevel()` call, should be treated as a constant otherwise, its lifespan should not exceed beyond the scope of the aforementioned classmethod.
     * @see <interface.h> IngameMapHandler::loadLevel()
    */
    struct Data_TilelayerTilesets {
        void load(json const& JSONLevelData, SDL_Renderer* renderer);
        Data_TilelayerTileset const* operator[](GID gid) const;

        private:
            std::vector<Data_TilelayerTileset> mData;
    };

    /**
     * @brief Contain data associated with a tileset for an entity or an animated object.
     * @param animationMapping maps an animation type to the associated data.
     * @param animationUpdateRate the number of frames a sprite should last before switching to the next. Should be treated as a constant.
     * @param animationSize represents the ratio between the size of one single animation/sprite and the size of a `Tile` on the tileset, per dimension. Should be implemented alongside `globals::tileDestSize`.
     * @note `clear()` method unnecessary since its lifespan (and its dependencies') should persist along with an `AbstractAnimatedEntity<T>`-derived as a static member.
    */
    struct Data_EntityTileset : public Data_Generic {
        /**
         * Register animation types as enumeration constants for maintainability.
        */
        enum class Animation : char {
            null = -1,

            kIdle,
            kAttackMeele,
            kAttackRanged,
            kDeath,
            kRun,
            kWalk,
            kDamaged,
        };

        Animation stoan(std::string const& s);

        /**
         * @brief Contain data associated with an animation i.e. a series of sprites.
         * 
         * @param startGID the first `GID` of the animation. Defaults to `0`.
         * @param stopGID the last `GID` of the animation. Defaults to `0`.
         * @param isPermanent specifies whether the animation should be a loop i.e. whether external calls should be performed when the animation reaches its end. Defaults to `false`.
        */
        struct Data_Animation {
            void load(pugi::xml_node const& XMLAnimationNode);

            int startGID = 0;
            int stopGID = 0;
            bool isPermanent = false;
            double updateRateMultiplier = 1;
        };

        void load(pugi::xml_document const& XMLTilesetData, SDL_Renderer* renderer);
        Data_Animation const& operator[](Animation animation) const;

        int animationUpdateRate = 64;
        SDL_Point animationSize = {1, 1};

        private:
            std::unordered_map<Animation, Data_Animation> mUMap;
    };
}


/**
 * @brief Group components that are associated to level and level-loading.
*/
namespace level {
    /**
     * Register level names as enumeration constants for maintainability.
    */
    enum class Name : char {
        null = -1,

        kLevelEquilibrium,
        kLevelValleyOfDespair,

        kLevelWhiteSpace,
    };

    Name stoln(std::string const& s);

    struct Map {
        void load(json const& JSONLevelMapData);
        inline void clear() { mUMap.clear(); }

        std::filesystem::path operator[](Name ln) const;   // Supports only index-based search

        private:
            std::unordered_map<Name, std::string> mUMap;
    };

    /**
     * @brief Contain data associated with an entity, used in level-loading.
     * @param destCoords the new `destCoords` of the entity upon entering new level.
    */
    struct Data_Generic {
        virtual ~Data_Generic() = default;   // Virtual destructor, required for polymorphism
        virtual void load(json const& JSONObjectData);
        
        SDL_Point destCoords;
    };

    struct Data_Interactable : public Data_Generic {
        void load(json const& JSONObjectData) override;

        std::vector<std::vector<std::string>> dialogues;
    };

    /**
     * @brief Contain data associated with a teleporter-type entity, used in level-loading.
     * @param targetDestCoords the new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
     * @param targetLevel the new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
    */
    struct Data_Teleporter : public Data_Generic {
        void load(json const& JSONObjectData) override;

        SDL_Point targetDestCoords;
        level::Name targetLevel;
    };

    struct Data {
        Data() = default;
        ~Data() { clear(); }

        std::vector<Data_Generic*> get(std::string const& key);
        void insert(std::string const& key, Data_Generic* data);
        void erase(std::string const& key);

        std::string getProperty(std::string const& key);
        void setProperty(std::string const& key, std::string const& property);
        void eraseProperty(std::string const& key);

        void load(json const& JSONLevelData);
        void clear();

        tile::Tensor tiles;
        tile::Data_TilelayerTilesets tilesets;

        SDL_Point tileDestSize;
        SDL_Point tileDestCount;
        SDL_Color backgroundColor;

        std::unordered_map<std::string, std::vector<Data_Generic*>> dependencies;
        std::unordered_map<std::string, std::string> properties;

        private:
            void loadMembers(json const& JSONLevelData);
            void loadTileLayer(json const& JSONLayerData);
            void loadObjectLayer(json const& JSONLayerData);
            void loadTilelayerTilesets(json const& JSONLevelData);
    };

    extern Data data;
}


using Animation = tile::Data_EntityTileset::Animation;


/**
 * @brief Group components that are associated to custom events.
*/
namespace event {
    /**
     * @note Does not implicitly convert to underlying integer type `Sint32`.
     * @note Strictly abide to the following format: `k[Req/Resp]_[AnimationType]_[SrcEntityType]_[DestEntityType]`
    */
    enum class Code : Sint32 {
        // Uses `event::ID`
        kReq_DeathPending_Player,
        kReq_DeathFinalized_Player,

        // Uses `Generic`
        kReq_AttackRegister_Player_GHE,
        kReq_AttackRegister_GHE_Player,
        kReq_AttackInitiate_GHE_Player,
        kReq_MoveInitiate_GHE_Player,
        kResp_AttackInitiate_GHE_Player,
        kResp_MoveInitiate_GHE_Player,
        kResp_MoveTerminate_GHE_Player,

        // Uses `Interactable`
        kReq_Interact_Player_GIE,

        // Uses `Teleporter`
        kReq_Teleport_GTE_Player,
        kResp_Teleport_GTE_Player,
    };

    using ID = int;

    struct Data_Generic {
        SDL_Point destCoords;
        SDL_Point range;
        EntitySecondaryStats stats;
    };

    struct Data_Interactable {
        SDL_Point targetDestCoords;
    };

    struct Data_Teleporter {
        SDL_Point destCoords;
        SDL_Point targetDestCoords;
        level::Name targetLevel;
    };

    /**
     * The global custom event type. Registered only once.
    */
    extern uint32_t type;
    void initialize();

    SDL_Event instantiate();
    void terminate(SDL_Event const& event);
    void enqueue(SDL_Event& event);

    event::ID getID(SDL_Event const& event);
    void setID(SDL_Event& event, event::ID id);
    event::Code getCode(SDL_Event const& event);
    void setCode(SDL_Event& event, event::Code code);

    /**
     * Helper function. Not meant to be used outside of namespace.
    */
    template <typename Data>
    inline void __deallocate__(SDL_Event const& event) {
        if (event.user.data1 != nullptr) delete reinterpret_cast<Data*>(event.user.data1);
    }

    /**
     * @note Should only be called when `event.user.data1` is not `nullptr`.
    */
    template <typename Data>
    inline Data getData(SDL_Event const& event) {
        return event.user.data1 != nullptr ? *reinterpret_cast<Data*>(event.user.data1) : Data{};
    }

    /**
     * @note Assumes that `event.user.data1`, for its entire lifespan, is of type `Data`.
    */
    template <typename Data>
    inline void setData(SDL_Event& event, Data const& data) {
        if (event.user.data1 != nullptr) delete reinterpret_cast<Data*>(event.user.data1);
        event.user.data1 = new Data(data);
    }
}


/**
 * @brief Group components that are used for configuration.
 * @note Recommended implementation: components should be retrieved once in `Game` initialization and may extend to other retrieval operations, manipulation is not possible.
 * @see https://sourceforge.net/p/precleardef/wiki/OperatingSystems/
*/
namespace config {
    constexpr bool enable_audio = true;

    /**
     * Uses `operator~` for static conversion to `SDL_Keycode`.
    */
    enum class Key : SDL_Keycode {
        kExit = SDLK_ESCAPE,

        kIngameReturnMenu = SDLK_F1,
        kIngameDialogueTest = SDLK_F2,
        kIngameLevelReset = SDLK_F4,
        kIngameCameraAngleToggle = SDLK_F5,
        kIngameGrayscaleToggle = SDLK_F6,

        kAffirmative = SDLK_e,
        kNegative = SDLK_q,

        kPlayerMoveUp = SDLK_w,
        kPlayerMoveDown = SDLK_s,
        kPlayerMoveRight = SDLK_a,
        kPlayerMoveLeft = SDLK_d,
        kPlayerRunToggle = SDLK_LSHIFT,
        kPlayerAttackMeele = SDLK_SPACE,
        kPlayerAttackSurgeProjectileOrthogonalSingle = SDLK_1,
        kPlayerAttackSurgeProjectileOrthogonalDouble = SDLK_2,
        kPlayerAttackSurgeProjectileOrthogonalTriple = SDLK_3,
        kPlayerAttackSurgeProjectileOrthogonalQuadruple = SDLK_4,
        kPlayerAttackSurgeProjectileDiagonalQuadruple = SDLK_5,
    };

    constexpr SDL_Keycode operator~(Key key) { return static_cast<SDL_Keycode>(key); }

    namespace path {
        const std::filesystem::path asset = "assets";
        const std::filesystem::path asset_tiled = asset / ".tiled";
        const std::filesystem::path asset_font = asset / "fonts";
        const std::filesystem::path asset_audio = asset / "audio";
        
        namespace font {
            const std::filesystem::path OmoriChaotic = asset_font / "omori-game-1.ttf";
            const std::filesystem::path OmoriHarmonic = asset_font / "omori-game-2.ttf";
            const std::filesystem::path Phorssa = asset_font / "phorssa.ttf";   // Bizarre
        }
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
            config::color::smoky_black, config::color::offwhite, config::color::offwhite, 0.125f / 32.0f, 1.0f / 32.0f,
        };
        constexpr ComponentPreset dialogue = {
            config::color::black, config::color::offwhite, config::color::offwhite, 0.375f / 32.0f, 1.25f / 32.0f,
        };
        constexpr ComponentPreset title = {
            config::color::transparent, config::color::transparent, config::color::offwhite, 0, 0,
        };
    }

    namespace game {
        constexpr int FPS = 60;
        const std::filesystem::path windowIconPath = config::path::asset / "icon/light.png";

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
            FPS,
            "",
        };
    }

    namespace mixer {
        // Different initialization process
        constexpr int frequency = MIX_DEFAULT_FREQUENCY;
        constexpr uint16_t format = MIX_DEFAULT_FORMAT;
        constexpr int channels = MIX_DEFAULT_CHANNELS;
        constexpr int chunkSize = 2048;   // 2 KB

        constexpr int masterVolume = 66;
        constexpr int BGM_Volume = 25;
        constexpr int SFX_Volume = 50;
    }

    namespace interface {
        const std::filesystem::path path = "assets/.tiled/levels.json";
        constexpr level::Name levelName = level::Name::kLevelWhiteSpace;
        constexpr int idleFrames = 16;

        constexpr double tileCountHeight = 24;   // OMORI's white space
        constexpr double grayscaleIntensity = 0.5;
    }

    /**
     * @see https://stackoverflow.com/questions/54258241/warning-iso-c-forbids-converting-a-string-constant-to-char-for-a-static-c
    */
    namespace entities {
        constexpr double runVelocityModifier = 4;
        constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
        
        namespace player {
            constexpr const char* typeID = "player";
            const std::vector<std::filesystem::path> paths = {
                "assets/.tiled/.tsx/player-premade-01.tsx",
                "assets/.tiled/.tsx/player-premade-02.tsx",
                "assets/.tiled/.tsx/player-premade-03.tsx",
                "assets/.tiled/.tsx/player-premade-04.tsx",
                "assets/.tiled/.tsx/player-premade-05.tsx",
                "assets/.tiled/.tsx/player-premade-06.tsx",
                "assets/.tiled/.tsx/player-premade-07.tsx",
                "assets/.tiled/.tsx/player-premade-08.tsx",
                "assets/.tiled/.tsx/player-premade-09.tsx",
                "assets/.tiled/.tsx/player-premade-10.tsx",
                "assets/.tiled/.tsx/player-premade-11.tsx",
                "assets/.tiled/.tsx/player-premade-12.tsx",
                "assets/.tiled/.tsx/player-premade-13.tsx",
                "assets/.tiled/.tsx/player-premade-14.tsx",
                "assets/.tiled/.tsx/player-premade-15.tsx",
                "assets/.tiled/.tsx/player-premade-16.tsx",
                "assets/.tiled/.tsx/player-premade-17.tsx",
                "assets/.tiled/.tsx/player-premade-18.tsx",
                "assets/.tiled/.tsx/player-premade-19.tsx",
                "assets/.tiled/.tsx/player-premade-20.tsx",
            };
            const std::filesystem::path path = paths[0];
            constexpr SDL_FRect destRectModifier = { 0, -1.125, 1, 1 };
            constexpr SDL_FPoint velocity = { 16, 16 };
            constexpr int moveDelay = 0;
            constexpr SDL_Point attackRegisterRange = { 99, 99 };
            constexpr EntityPrimaryStats primaryStats = { 10, 10, 10, 10, 10, 10, 10, 10 };
            constexpr int waitingFramesAfterDeath = 6.66 * config::game::FPS;
        }

        namespace interactable {
            constexpr const char* typeID = "interactable";
            const std::filesystem::path path{};
            constexpr SDL_FRect destRectModifier{};
        }

        namespace teleporter {
            constexpr const char* typeID = "teleporter";
            const std::filesystem::path path = "assets/.tiled/.tsx/mi-a-cat.tsx";
            constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
        }

        namespace teleporter_red_hand_throne {
            constexpr const char* typeID = "teleporter-red-hand-throne";
            const std::filesystem::path path = "assets/.tiled/.tsx/omori-red-hand-throne.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -0.25, 1, 1 };
        }

        namespace slime {
            constexpr const char* typeID = "slime";
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
            constexpr const char* typeID = "pentacle-projectile";
            const std::filesystem::path path = "assets/.tiled/.tsx/mi-a-pentacle.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -1, 1, 1 };
            constexpr SDL_FPoint velocity = { 0, 0 };
            constexpr int moveDelay = 0;
            constexpr SDL_Point attackRegisterRange = { 1, 1 };
            constexpr EntityPrimaryStats primaryStats = { 0, 0, 0, 0, 0, 0, 10, 0 };
        }

        namespace omori_laptop {
            constexpr const char* typeID = "interactable-omori-laptop";
            const std::filesystem::path path = "assets/.tiled/.tsx/omori-laptop.tsx";
            constexpr SDL_FRect destRectModifier = { 0, -0.125, 1, 1 };
        }

        namespace omori_light_bulb {
            constexpr const char* typeID = "omori-light-bulb";
            const std::filesystem::path path = "assets/.tiled/.tsx/omori-light-bulb.tsx";
            constexpr SDL_FRect destRectModifier = { 0.5, 0, 1, 1 };
        }

        namespace omori_mewo {
            constexpr const char* typeID = "interactable-omori-mewo";
            const std::filesystem::path path = "assets/.tiled/.tsx/omori-mewo.tsx";
            constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
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
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.1f, 0.05f }, config::preset::title, "quitting~");
            constexpr double destSizeModifier = 0.5;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
            constexpr double progressUpdateRateLimit = 1;
            constexpr double progressUpdateRate = progressUpdateRateLimit / static_cast<double>(config::game::FPS >> 2);
        }

        namespace dialogue_box {
            const std::tuple<SDL_FPoint, ComponentPreset> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 0.85f }, config::preset::dialogue);
            constexpr double destSizeModifier = 2.25;
            constexpr SDL_Point destRectRatio = { 5, 1 };
            constexpr double destOffsetRatio = 0.1;
            const std::filesystem::path fontPath = config::path::font::OmoriHarmonic;
            constexpr unsigned short int delayCounterLimit = config::game::FPS >> 2;

            const std::vector<std::string> test = {
                "Steady your heartbeat....\nDon't be afraid. It's not as scary as you think.",
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
                "Thread 1 received signal SIGTRAP, Trace/breakpoint trap. 0x00007fff6bf7c733 in ntdll!RtlIsZeroMemory () from C:\\WINDOWS\\SYSTEM32\\ntdll.dll",
                "Apathy's a tragedy, and boredom is a crime.\nAnything and everything, all of the time.",
            };
        }

        namespace menu_avatar {
            constexpr double destRectModifier = 0.125;
        }

        namespace menu_parallax {
            constexpr double animationUpdateRate = 0.001;
            const std::filesystem::path path = "assets/graphics/parallaxes/omori-static.png";
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
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 0.2f }, config::preset::title, "introspection");
            constexpr double destSizeModifier = 3.0;
            constexpr SDL_Point destRectRatio = config::components::destRectRatio;
            const std::filesystem::path fontPath = config::path::font::Phorssa;
        }

        namespace loading_message {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.5f, 4.0f / 9.0f }, config::preset::title, "loading");
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
     * The current position of the mouse relative to the window.
    */
    extern SDL_Point mouseState;

    extern GameState state;
}


bool operator==(SDL_Point const& first, SDL_Point const& second);
bool operator!=(SDL_Point const& first, SDL_Point const& second);
bool operator<(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator+(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator-(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator-(SDL_Point const& instance);
SDL_Point operator*(std::array<std::array<int, 2>, 2> matrix, SDL_Point const& vector);
SDL_Point operator~(SDL_Point const& instance);
SDL_Point operator<<(SDL_Point const& instance, unsigned int times);
SDL_Point operator>>(SDL_Point const& instance, unsigned int times);

bool operator==(SDL_FPoint const& first, SDL_FPoint const& second);
SDL_FPoint operator<<(SDL_FPoint const& instance, float rad);
SDL_FPoint operator>>(SDL_FPoint const& instance, float rad);

/**
 * @brief Enable `switch` on `std::string`.
 * @note Abbreviation for "hash(ed) string".
 * @see https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
*/
constexpr unsigned int hs(const char* s, int hash = 0) {
    return !s[hash] ? 5381 : (hs(s, hash + 1) * 33) ^ s[hash];
}

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

    SDL_Texture* duplicateTexture(SDL_Renderer* renderer, SDL_Texture* texture);
    SDL_Texture* createGrayscaleTexture(SDL_Renderer* renderer, SDL_Texture* texture, double intensity = 1);

    template <typename T>
    std::vector<T> zlibDecompress(std::string const& s);
    std::string base64Decode(std::string const& s);

    void readJSON(std::filesystem::path const& path, json& data);
    std::filesystem::path cleanRelativePath(std::filesystem::path const& path);

    void setTextureRGB(SDL_Texture*& texture, SDL_Color const& color);
    void setTextureRGBA(SDL_Texture*& texture, SDL_Color const& color);  
}


#endif