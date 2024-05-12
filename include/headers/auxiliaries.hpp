#ifndef AUXILIARIES_H
#define AUXILIARIES_H

#include <iostream>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <optional>
#include <list>
#include <limits>
#include <queue>
#include <string>
#include <string_view>
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

#include <garbage-collector.hpp>


/**
 * Register JSON for Modern C++ i.e. `nlohmann::json` as the only JSON library in the scope of this project.
*/
using json = nlohmann::json;


/* Compile-time Utilities */

/**
 * @brief Enable `constexpr`-ness on `std::string` to be used in `switch` statements.
 * @note Abbreviation for "hash(ed) string".
 * @see https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
*/
constexpr inline unsigned int hstr(const char* s, int hashval = 0) {
    return !s[hashval] ? 5381 : (hstr(s, hashval + 1) * 33) ^ s[hashval];
}

/**
 * @brief Blend 2 colors.
 * @param bi The blend intensity of the first color.
 * @see https://gist.github.com/quantum9Innovation/33f2f709b59c3fe71b976aa738fb1327
 * @see https://commit-digest.kde.org/issues/2007-08-12/

*/
constexpr inline SDL_Color blend(SDL_Color const& lhs, SDL_Color const& rhs, double bi = 0.5) {
    constexpr auto max = [](SDL_Color const& col) -> Uint8 {
        return std::max(col.r, std::max(col.g, col.b));
    };

    std::clamp(bi, (double)(0), (double)(1));   // Yes this is constexpr

    SDL_Color ret({
        static_cast<Uint8>(bi * lhs.r + (1 - bi) * rhs.r),
        static_cast<Uint8>(bi * lhs.g + (1 - bi) * rhs.g),
        static_cast<Uint8>(bi * lhs.b + (1 - bi) * rhs.b),
        static_cast<Uint8>(bi * lhs.a + (1 - bi) * rhs.a),
    });

    double scale = max(ret) / ((2 * bi * max(lhs) + (2 - 2 * bi) * max(rhs)) / 2);

    ret.r /= scale;
    ret.g /= scale;
    ret.b /= scale;

    return ret;
}

/**
 * @brief The C++17's way of concatenating `const char*` (uses `std::string_view`).
 * @see https://stackoverflow.com/questions/38955940/how-to-concatenate-static-strings-at-compile-time
*/
template <std::string_view const&... Strs>
struct strccat
{
    private:
        // Concatenate all strings into an `std::array<char>`
        static constexpr auto impl() noexcept
        {
            constexpr std::size_t size = (Strs.size() + ...);
            std::array<char, size + 1> arr{};

            auto append = [i = 0, &arr](const auto& s) mutable {
                for (auto c : s) arr[i++] = c;
            };

            (append(Strs), ...);
            arr[size] = 0;

            return arr;
        }

        // Static storage for the concatenated string
        static constexpr auto mRepr = impl();

    public:
        // View as a `std::string_view`
        static constexpr std::string_view value { mRepr.data(), mRepr.size() - 1 };
};

// template <std::string_view const&... Strs>
// static constexpr auto strccat_v = strccat<Strs...>::value;


/* Enumerations & Structs */

/**
 * @note "No Interface" states are not governed by an `AbstractInterface<T>`-derived, therefore its implementation in `Game::handleDependencies()` should end with a follow-up to a non-`kNITF` state.
 * @note "Prevent Warning" states only exist to prevent compiler warnings. Do not use those.
*/
enum class GameState : unsigned short int {
    kExit = 1,
    kMenu = 2,
    kLoading = 4,
    kIngamePlaying = 8,
    kIngameDialogue = 16,
    kGameOver = 32,

    /* "No Interface" states */
    kNITF_NewGame,
    kNITF_Continue,

    /* "Prevent Warning" states */
    kPWRN_6 = 6,
    kPWRN_12 = 12,
};

constexpr GameState operator|(GameState const& first, GameState const& second) {
    return static_cast<GameState>(static_cast<int>(first) | static_cast<int>(second));   // Must be defined here
}

/**
 * @brief Provide flexible handling base on an entity movement's status.
*/
enum class BehaviouralType : unsigned char {
    kDefault,
    kInvalidated,
    kContinued,
    kPrioritized,
    kAutopilot,   // Custom mix of `kInvalidated` and `kContinued`
};

enum class MovementSelectionType : unsigned char {
    kGreedyTrigonometric,
    kGreedyRandomBinary,
    kPathfindingAStar,
    // kPathfindingDijkstra,
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
 * @see https://learn.microsoft.com/en-us/cpp/error-messages/compiler-errors-2/compiler-error-c7510?view=msvc-170
*/
class EntityAttributes {
public:
    enum class ID {
        HP, MP, ATK, DEF,
        MHP, MMP, MAT, MDF,
        MIR, AIR, ARR,
    };
    
private:
    friend constexpr inline ID operator+(ID lhs, ID rhs) { return static_cast<ID>(static_cast<int>(lhs) + static_cast<int>(rhs)); }
    friend constexpr inline ID operator-(ID lhs, ID rhs) { return static_cast<ID>(static_cast<int>(lhs) - static_cast<int>(rhs)); }
    friend constexpr inline ID& operator++(ID& id) { id = static_cast<ID>(static_cast<int>(id) + 1); return id; }   // Prefix increment

    template <ID idl, ID idr, typename T = int, std::size_t N = static_cast<std::size_t>(idr) - static_cast<std::size_t>(idl) + 1>   // Template parameter `N` helps prevent compilation errors and should be automatically deduced, not provided
    struct Base {
        template <ID id, typename U = void>
        using pred = typename std::enable_if<idl <= id && id <= idr, U>::type;

        constexpr inline Base() = default;
        constexpr inline Base(std::array<T, N> const& attrs) : mAttrs(attrs) {}

        template <ID id> pred<id, T const&>
        inline get() const { return mAttrs[static_cast<int>(id - idl)]; }

        constexpr inline std::array<T, N> const& get() const { return mAttrs; }

    protected:
        template <ID id> pred<id, T&>
        inline at() { return mAttrs[static_cast<int>(id - idl)]; }

        std::array<T, N> mAttrs;
    };

    struct Static final : public Base<ID::MHP, ID::MDF> {
        constexpr inline Static(std::array<int, 4> const& attrs = {{ 0, 0, 0, 0 }}) : Base<ID::MHP, ID::MDF>(attrs) {}

        inline void levelup() { for (auto& attr : mAttrs) attr *= mScale; }
        inline void leveldown() { for (auto& attr : mAttrs) attr /= mScale; }

        private:
            static constexpr double mScale = 1.5;
    };

    struct Dynamic final : public Base<ID::HP, ID::DEF> {
        static constexpr inline auto sttody(ID id) { return id - ID::MHP; }
        static constexpr inline auto dytost(ID id) { return id + ID::MHP; }

        constexpr inline Dynamic(Static const& st = {{ 0, 0, 0, 0 }}) : Base<ID::HP, ID::DEF>(st.get()) {}

        template <ID id> pred<id>
        inline heal(Static const& st, int val) { at<id>() = std::min(st.get<dytost(id)>(), at<id>() + val); }

        template <ID id> pred<id>
        inline heal(Static const& st) { return heal<id>(st.get<dytost(id)>() - at<id>()); }

        inline void heal(Static const& st) { mAttrs = st.get(); }

        // Returns `true` if the attack brings `HP` of `other` to `0` or below
        inline bool attack(Dynamic& other, int MP) {
            bool flag = other.get<ID::HP>() > 0;

            if (get<ID::MP>() < MP) return false;
            at<ID::MP>() -= MP;   // Attack consumes `MP`
            other.at<ID::HP>() -= std::max(get<ID::ATK>() - other.get<ID::DEF>(), 0);   // Classic formula
            if (other.get<ID::HP>() < 0) other.at<ID::HP>() = -1;

            return flag && other.at<ID::HP>() <= 0;
        }
    };

    struct Range final : public Base<ID::MIR, ID::ARR, SDL_Point> {
        constexpr inline Range(std::array<SDL_Point, 3> const& attrs = {{ { 0, 0 }, { 0, 0 }, { 0, 0 } }}) : Base<ID::MIR, ID::ARR, SDL_Point>(attrs) {}

        template <ID id> pred<id, bool>
        inline within(SDL_Point const& lpos, SDL_Point const& rpos) const {
            auto dist = std::sqrt(std::pow(lpos.x - rpos.x, 2) + std::pow(lpos.y - rpos.y, 2));
            return dist <= get<id>().x && dist <= get<id>().y;
        }
    };

    Static mStatic;
    Dynamic mDynamic;
    Range mRange;

public:
    constexpr inline EntityAttributes() : mStatic(), mDynamic(), mRange() {}
    constexpr inline EntityAttributes(Static const& st) : mStatic(st), mDynamic(st) {}
    constexpr inline EntityAttributes(Static const& st, Range const& rg) : mStatic(st), mDynamic(st), mRange(rg) {}

    inline ~EntityAttributes() = default;
    
    template <ID id> Range::pred<id, bool>
    inline within(SDL_Point const& pos, SDL_Point const& opos) const { return mRange.within<id>(pos, opos); }

    template <ID id> Dynamic::pred<id, void>
    inline heal(int val) { return mDynamic.heal<id>(mStatic, val); }

    template <ID id> Dynamic::pred<id, void>
    inline heal() { return mDynamic.heal<id>(mStatic); }

    inline void heal() { return mDynamic.heal(mStatic); }

    inline bool attack(EntityAttributes& other, int MP = 0) { return mDynamic.attack(other.mDynamic, MP); }
    inline bool attack(EntityAttributes& other, SDL_Point const& pos, SDL_Point const& opos, int MP = 0) { return within<ID::ARR>(pos, opos) ? attack(other, MP) : false; }

    template <ID id> Static::pred<id, int> inline get() const { return mStatic.get<id>(); }
    template <ID id> Dynamic::pred<id, int> inline get() const { return mDynamic.get<id>(); }
    template <ID id> Range::pred<id, SDL_Point> inline get() const { return mRange.get<id>(); }
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
        std::optional<Data_TilelayerTileset> operator[](GID gid) const;

        private:
            std::vector<Data_TilelayerTileset> mData;
    };
    
    /**
     * @brief Contain data associated with a tileset for an entity or an animated object.
     * @param animationMapping maps an animation type to the associated data.
     * @param animationTicks the number of frames a sprite should last before switching to the next. Should be treated as a constant.
     * @param animationSize represents the ratio between the size of one single animation/sprite and the size of a `Tile` on the tileset, per dimension. Should be implemented alongside `globals::tileDestSize`.
     * @note `clear()` method unnecessary since its lifespan (and its dependencies') should persist along with an `AbstractAnimatedEntity<T>`-derived as a static member.
    */
    struct Data_EntityTileset : public Data_Generic {
        /**
         * Register animation types as enumeration constants for maintainability.
        */
        enum class Animation : unsigned int {
            kIdle = hstr("animation-idle"),
            kWalk = hstr("animation-walk"),
            kRun = hstr("animation-run"),
            kAttackMeele = hstr("animation-attack-meele"),
            kAttackRanged = hstr("animation-attack-ranged"),
            kDamaged = hstr("animation-damaged"),
            kDeath = hstr("animation-death"),
        };

        static std::pair<std::optional<Animation>, std::optional<SDL_Point>> stoan(std::string const& s);

        static inline constexpr unsigned short int getPriority(Animation animation) {
            switch (animation) {
                case Animation::kIdle:
                case Animation::kWalk:
                case Animation::kRun:
                    return 0;
                
                case Animation::kAttackMeele:
                case Animation::kAttackRanged:
                    return 1;

                case Animation::kDamaged:
                    return 2;

                case Animation::kDeath:
                    return std::numeric_limits<unsigned short int>::max();

                default:
                    return 0;
            }
        }

        static inline constexpr bool getContinuity(Animation animation) {
            switch (animation) {
                case Animation::kIdle:
                case Animation::kWalk:
                case Animation::kRun:
                    return true;

                default: return false;
            }
        }

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
            double ticksMultiplier = 1;
        };

        static constexpr SDL_Point kDefaultDirection = { 1, 0 };

        void load(pugi::xml_document const& XMLTilesetData, SDL_Renderer* renderer);
        Data_Animation const& at(Animation animation, SDL_Point const& direction = kDefaultDirection) const;

        unsigned int animationTicks = 1000;
        SDL_Point animationSize = { 1, 1 };
        bool isMultiDirectional = false;

        private:
            struct hash {
                std::size_t operator()(std::pair<Animation, SDL_Point> const& instance) const;
            };
            
            struct equal_to {
                bool operator()(std::pair<Animation, SDL_Point> const& first, std::pair<Animation, SDL_Point> const& second) const;
            };

            std::unordered_map<std::pair<Animation, SDL_Point>, Data_Animation, hash, equal_to> mUMap;
    };
}


/**
 * @brief Group components that are associated to level and level-loading.
*/
namespace level {
    /**
     * Register level names as enumeration constants for maintainability.
    */
    enum class Name : unsigned int {
        kLevelPrelude = hstr("level-prelude"),
        kLevelWoodsEntryPoint = hstr("level-woods-entry-point"),
        kLevelWoodsLongLane = hstr("level-woods-long-lane"),
        kLevelWoodsMysteryShack = hstr("level-woods-mystery-shack"),
        kLevelWoodsCrossroadsFirst = hstr("level-woods-crossroads-first"),
        kLevelWoodsDeadEnd = hstr("level-woods-dead-end"),
        kLevelWoodsEnemyApproachingFirst = hstr("level-woods-enemy-approaching-first"),
        kLevelWoodsEnemyApproachingFinal = hstr("level-woods-enemy-approaching-final"),
        kLevelWoodsCrossroadsFinal = hstr("level-woods-crossroads-final"),
        kLevelWoodsDestinedDeath = hstr("level-woods-destined-death"),

        kLevelInterlude = hstr("level-interlude"),
        
        kLevelWhiteSpace = hstr("level-white-space"),
    };

    std::optional<Name> hstoln(unsigned int hs);
    inline std::optional<Name> stoln(std::string const& s) { return hstoln(hstr(s.c_str())); }

    struct Map {
        void load(json const& JSONLevelMapData);
        inline void clear() { mUMap.clear(); }

        std::optional<std::filesystem::path> operator[](Name ln) const;   // Supports only index-based search

        private:
            std::unordered_map<Name, std::string> mUMap;
    };

    /**
     * @brief Contain data associated with an entity, used in level-loading.
     * @param destCoords the new `destCoords` of the entity upon entering new level.
    */
    struct Data_Generic {
        Data_Generic() = default;
        explicit Data_Generic(SDL_Point const& destCoords) : destCoords(destCoords) {}
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
        Data_Teleporter() = default;
        Data_Teleporter(SDL_Point const& destCoords, SDL_Point const& targetDestCoords, level::Name targetLevel) : Data_Generic(destCoords), targetDestCoords(targetDestCoords), targetLevel(targetLevel) {}
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

        template <typename T = std::string>
        T getProperty(std::string const& key);
        template <typename T = std::string>
        void setProperty(std::string const& key, T const& property);
        void eraseProperty(std::string const& key);

        void load(json const& JSONLevelData);
        void clear();

        tile::Tensor tiles;
        tile::Data_TilelayerTilesets tilesets;
        std::vector<std::vector<tile::GID>> collisionTilelayer;
        SDL_Point autopilotTargetTile;   // For autopilot

        SDL_Point tileDestSize;
        SDL_Point tileDestCount;
        double viewportHeight;
        SDL_Color backgroundColor;

        std::unordered_map<std::string, std::vector<Data_Generic*>> dependencies;
        std::unordered_map<std::string, std::string> properties;

        private:
            void loadProperties(json const& JSONLevelData);
            void loadLayers(json const& JSONLevelData);
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
        EntityAttributes* attributes = nullptr;

        inline ~Data_Generic() = default;
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


namespace pathfinders {
    enum class Heuristic : unsigned char {
        kManhattan,   // Best for 4-directional
        kDiagonal,   // Best for 8-directional
        kChebyshev,   // Diagonal with `D = D2 = 1`
        kOctile,   // Diagonal with `D = 1, D2 = std::sqrt(2)`
        kEuclidean,   // Works for any directions
        kConstantZero,   // `g << h`, reverts into Dijkstra
        kContantInf,   // `g >> h`, reverts into Greedy Best-First-Search
    };

    enum class MovementType : bool {
        k4Directional,
        k8Directional,
    };

    enum class Status : unsigned char {
        kInvalidSrc,
        kInvalidDest,
        kBlockedSrc,
        kBlockedDest,
        kCoincidents,

        kSuccess,
        kFailure,
    };

    struct Cell {
        int x, y;

        inline bool operator==(Cell const& other) const { return x == other.x && y == other.y; }
        inline Cell operator+(Cell const& other) const { return { x + other.x, y + other.y }; }
        friend inline std::ostream& operator<<(std::ostream& os, Cell const& self) { return os << '(' << self.x << ", " << self.y << ')'; }

        static inline SDL_Point cltopt(Cell const& self) { return { self.x, self.y }; }
        static inline Cell pttocl(SDL_Point const& pt) { return { pt.x, pt.y }; }

        static double getG(Cell const& distance);

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kManhattan, double>
        static getH(Cell const& lhs, Cell const& rhs);

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kChebyshev, double>
        static getH(Cell const& lhs, Cell const& rhs);

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kOctile, double>
        static getH(Cell const& lhs, Cell const& rhs);

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kEuclidean, double>
        static getH(Cell const& lhs, Cell const& rhs);

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kConstantZero, double>
        static constexpr inline getH(Cell const& lhs, Cell const& rhs) { return 0; }

        template <Heuristic H>
        typename std::enable_if_t<H == Heuristic::kContantInf, double>
        static constexpr inline getH(Cell const& lhs, Cell const& rhs) { return std::pow(10, 307); }   // Might cause overflow

        // Forward declaration to prevent "incomplete type" compilation error
        struct Data;
        struct Pair;

        private:
            template <Heuristic H, unsigned short int Dsq, unsigned short int D2sq>   // Floating-point template parameter is nonstandardC/C++(605)
            typename std::enable_if_t<H == Heuristic::kDiagonal, double>
            static getH(Cell const& lhs, Cell const& rhs);
    };

    struct Cell::Data {
        Cell parent;
        double g, h, f = std::numeric_limits<double>::max();
    };

    struct Cell::Pair {
        Cell cell;
        double f;

        inline Pair(Cell const& cell, double f = 0) : cell(cell), f(f) {}
        inline bool operator>(Pair const& other) const { return f > other.f; }
        inline bool operator==(Pair const& other) const { return cell == other.cell && f == other.f; }
    };

    struct Result {
        const Status status;
        std::stack<Cell> path;

        Result(Status status, std::stack<Cell> path = {}) : status(status), path(path) {}
    };

    /**
     * @brief Used for the Open List in A* Search Algorithm implementation.
     * @note Supports insertion and min element removal at O(log(n)) time complexity.
    */
    template <typename T, typename Compare = std::greater<T>>
    class OpenList {
        public:
            inline OpenList() = default;
            inline ~OpenList() = default;
            
            void push(T const&);
            T pop();
            
            inline bool empty() const { return mHeap.empty(); }
            
        private:
            T const& getRoot() const;
            void eraseRoot();

            std::priority_queue<T, typename std::vector<T>, Compare> mHeap;
    };

    /**
     * @brief Implementation of A* pathfinding algorithm.
     * @see https://www.geeksforgeeks.org/a-search-algorithm/
    */
    template <Heuristic H = Heuristic::kManhattan, MovementType M = MovementType::k4Directional>
    class ASPF {
        public:
            ASPF(std::vector<std::vector<int>> const& grid);
            ~ASPF() = default;

            void setBegin(Cell const& begin);
            void setEnd(Cell const& end);

            Result search(Cell const& src, Cell const& dest) const;

        private:
            bool isValid(Cell const& cell) const;
            bool isUnblocked(Cell const& cell) const;
            bool isUnblocked(Cell const& parent, Cell const& successor) const;

            std::stack<Cell> getPath(std::vector<std::vector<Cell::Data>> const& cellData, Cell const& dest) const;

            static inline constexpr auto getDirections() {
                if constexpr(M == MovementType::k4Directional) {
                    return std::array<Cell, 4>{{
                        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
                    }};
                } else {
                    return std::array<Cell, 8>{{
                        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
                        { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 },
                    }};
                }
            }

            std::vector<std::vector<int>> const& mGrid;
            Cell mBegin, mEnd;

            static inline constexpr auto mDirections = getDirections();
    };

    /**
     * @brief Implementation of Dijkstra pathfinding algorithm (reverted from A* search algorithm by setting heuristic `h` to constant `0`).
     * @see https://cs.stackexchange.com/questions/83618/best-heuristic-for-a
     * @see https://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
    */
    template <MovementType M = MovementType::k4Directional>
    using DJPF = ASPF<Heuristic::kConstantZero, M>;

    /**
     * @brief Implementation of Greedy Best-First-Search pathfinding algorithm (reverted from A* search algorithm by setting heuristic `h` to be much higher than `g` so that `g` does not contribute to `f`).
     * @see https://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
    */
    template <MovementType M = MovementType::k4Directional>
    using GBFSPF = ASPF<Heuristic::kContantInf, M>;
};


/**
 * @brief Group components that are used for configuration.
 * @note Recommended implementation: components should be retrieved once in `Game` initialization and may extend to other retrieval operations, manipulation is not possible.
 * @see https://sourceforge.net/p/precleardef/wiki/OperatingSystems/
*/
namespace config {
    constexpr bool enable_audio = true;
    constexpr bool enable_entity_overlap = true;

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
        kPlayerAutopilotToggle = SDLK_TAB,
        kPlayerRunToggle = SDLK_LSHIFT,
        kPlayerAttackMeele = SDLK_SPACE,
        kPlayerAttackSurgeProjectileOrthogonalSingle = SDLK_1,
        kPlayerAttackSurgeProjectileOrthogonalDouble = SDLK_2,
        kPlayerAttackSurgeProjectileOrthogonalTriple = SDLK_3,
        kPlayerAttackSurgeProjectileOrthogonalQuadruple = SDLK_4,
        kPlayerAttackSurgeProjectileDiagonalQuadruple = SDLK_5,
        kPlayerAttackMeteorShower = SDLK_j,
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
        constexpr SDL_Color dark = SDL_Color{ 0x32, 0x2f, 0x27, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color light = SDL_Color{ 0xb1, 0xae, 0xa8, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color offwhite = SDL_Color{ 0xf2, 0xf3, 0xf4, SDL_ALPHA_OPAQUE };
        constexpr SDL_Color offblack = { 0x14, 0x14, 0x12, SDL_ALPHA_OPAQUE };

        constexpr SDL_Color lighter = blend(light, offwhite, 0.66);
        constexpr SDL_Color halftone = blend(light, dark);
        constexpr SDL_Color darker = blend(dark, offblack, 0.66);

        constexpr SDL_Color transparent = SDL_Color{ 0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT };
    }

    namespace preset {
        constexpr ComponentPreset light = {
            config::color::lighter, config::color::darker, config::color::darker, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset dark = {
            config::color::darker, config::color::lighter, config::color::lighter, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset offwhite = {
            config::color::darker, config::color::offwhite, config::color::offwhite, 1.0f / 32.0f, 4.0f / 32.0f,
        };
        constexpr ComponentPreset FPSOverlay = {
            config::color::darker, config::color::offwhite, config::color::offwhite, 0.125f / 32.0f, 1.0f / 32.0f,
        };
        constexpr ComponentPreset dialogue = {
            config::color::darker, config::color::lighter, config::color::lighter, 0.375f / 32.0f, 1.25f / 32.0f,
        };
        constexpr ComponentPreset title = {
            config::color::transparent, config::color::transparent, config::color::lighter, 0, 0,
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
                SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED,
                {
                    { SDL_HINT_RENDER_SCALE_QUALITY, "0" },
                    { SDL_HINT_RENDER_VSYNC, "1" },
                    { SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4, "1" },
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
        const std::filesystem::path savePath = "build/save/autosave.json";
        const std::filesystem::path levelPath = "assets/.tiled/levels.json";
        constexpr level::Name levelName = level::Name::kLevelPrelude;
        constexpr int idleFrames = 16;
        constexpr std::size_t LRUCacheSize = 64;

        constexpr double viewportHeight = 10;
        constexpr double grayscaleIntensity = 1;
    }

    /**
     * @see https://stackoverflow.com/questions/54258241/warning-iso-c-forbids-converting-a-string-constant-to-char-for-a-static-c
    */
    namespace entities {
        constexpr double runVelocityModifier = 4;
        constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
        constexpr unsigned int SFXTicks = 777;
        constexpr unsigned int ASPFTicks = 1111;
        
        namespace player {
            constexpr const char* typeID = "player";
            const std::vector<std::filesystem::path> paths = {
                "assets/.tiled/.tsx/hana-caraka.tsx",
                "assets/.tiled/.tsx/hana-caraka-reverse.tsx",
            };
            const std::filesystem::path path = paths.front();
            constexpr SDL_FRect destRectModifier = { 0, -1, 4, 4 };
            constexpr SDL_FPoint velocity = { 16, 16 };
            constexpr unsigned int moveDelayTicks = 0;
            constexpr EntityAttributes attributes({{ 10, 4, 2, 1 }}, {{ SDL_Point{ 0, 0 }, { 0, 0 }, { 4, 4 } }});

            constexpr unsigned int deathTicks = 6666;
            constexpr unsigned int rangedAttackCooldownTicks = 1000;
            constexpr unsigned int meteorShowerAttackCooldownTicks = 4444;
        }

        namespace placeholders {
            namespace interactable {
                constexpr const char* typeID = "interactable";
                const std::filesystem::path path{};
                constexpr SDL_FRect destRectModifier{};
            }

            namespace teleporter {
                constexpr const char* typeID = "teleporter";
                const std::filesystem::path path{};
                constexpr SDL_FRect destRectModifier{};                
            }
        }

        namespace interactables {
            namespace omori_keeper {
                constexpr const char* typeID = "interactable-omori-keeper";
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-keeper.tsx";
                constexpr SDL_FRect destRectModifier = { -1, -1.5, 4, 4 };
            }

            namespace omori_pinwheel {
                constexpr const char* typeID = "omori-pinwheel";
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-pinwheel.tsx";
                constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
            }

            namespace omori_pinwheel_mono {
                constexpr const char* typeID = "omori-pinwheel-mono";
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-pinwheel-mono.tsx";
                constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
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

            #define DEF_OMORI_CAT_NS(index) \
            namespace omori_cat_##index {\
                constexpr const char* typeID = "interactable-omori-cat-" #index;\
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-cat-" #index ".tsx";\
                constexpr SDL_FRect destRectModifier = { 0.22, -0.5, 1.44, 1.44 };\
            }

            DEF_OMORI_CAT_NS(0)
            DEF_OMORI_CAT_NS(1)
            DEF_OMORI_CAT_NS(2)
            DEF_OMORI_CAT_NS(3)
            DEF_OMORI_CAT_NS(4)
            DEF_OMORI_CAT_NS(5)
            DEF_OMORI_CAT_NS(6)
            DEF_OMORI_CAT_NS(7)

            namespace omori_keys_wasd {
                constexpr const char* typeID = "omori-keys-wasd";
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-keys-wasd.tsx";
                constexpr SDL_FRect destRectModifier = { 0.4921875, 0.4921875, 2, 2 };
            }
        }

        namespace teleporter {
            namespace red_hand_throne {
                constexpr const char* typeID = "teleporter-red-hand-throne";
                const std::filesystem::path path = "assets/.tiled/.tsx/omori-red-hand-throne.tsx";
                constexpr SDL_FRect destRectModifier = { 0, -0.25, 1, 1 };                
            }
        }

        namespace hostile {
            namespace crab {
                constexpr const char* typeID = "hostile-crab";
                const std::filesystem::path path = "assets/.tiled/.tsx/egi-crab.tsx";
                constexpr SDL_FRect destRectModifier = { 0, 0, 6, 6 };
                constexpr SDL_FPoint velocity = { 64, 64 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ 4, 0, 2, 3 }}, {{ SDL_Point{ 6, 6 }, { 2, 2 }, { 2, 2 } }});
            }

            namespace crab_big {
                constexpr const char* typeID = "hostile-crab-big";
                const std::filesystem::path path = "assets/.tiled/.tsx/egi-crab.tsx";
                constexpr SDL_FRect destRectModifier = { 0, 0, 12, 12 };
                constexpr SDL_FPoint velocity = { 128, 128 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ std::numeric_limits<unsigned short int>::max(), 0, std::numeric_limits<unsigned short int>::max(), std::numeric_limits<unsigned short int>::max() }}, {{ SDL_Point{ std::numeric_limits<unsigned short int>::max(), std::numeric_limits<unsigned short int>::max() }, { 4, 4 }, { 4, 4 } }});
            }
        }

        namespace projectile {
            namespace darkness {
                constexpr const char* typeID = "projectile-darkness";
                const std::filesystem::path path = "assets/.tiled/.tsx/mta-darkness.tsx";
                constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
                constexpr SDL_FPoint velocity = { 0, 0 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ 0, 0, 2, 0 }}, {{ SDL_Point{ 0, 0 }, { 0, 0 }, { 1, 1 } }});
            }

            namespace slash {
                constexpr const char* typeID = "projectile-slash";
                const std::filesystem::path path = "assets/.tiled/.tsx/mta-slash.tsx";
                constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
                constexpr SDL_FPoint velocity = { 0, 0 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ 0, 0, 0, 0 }}, {{ SDL_Point{ 0, 0 }, { 0, 0 }, { 0, 0 } }});
            }

            namespace claw {
                constexpr const char* typeID = "projectile-claw";
                const std::filesystem::path path = "assets/.tiled/.tsx/mta-claw.tsx";
                constexpr SDL_FRect destRectModifier = { 0, 0, 1, 1 };
                constexpr SDL_FPoint velocity = { 0, 0 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ 0, 0, 0, 0 }}, {{ SDL_Point{ 0, 0 }, { 0, 0 }, { 0, 0 } }});
            }

            namespace meteor {
                constexpr const char* typeID = "projectile-meteor";
                const std::filesystem::path path = "assets/.tiled/.tsx/mta-meteor.tsx";
                constexpr SDL_FRect destRectModifier = { 0, -1.5, 3, 3 };
                constexpr SDL_FPoint velocity = { 0, 0 };
                constexpr int moveDelayTicks = 0;
                constexpr EntityAttributes attributes({{ 0, 0, std::numeric_limits<unsigned short int>::max(), 0 }}, {{ SDL_Point{ 0, 0 }, { 0, 0 }, { 4, 4 } }});
            }
        }

        namespace misc {
            namespace umbra {
                constexpr const char* typeID = "misc-umbra";
                const std::filesystem::path path = "assets/.tiled/.tsx/umbra.tsx";
                constexpr SDL_FRect destRectModifier = config::entities::destRectModifier;
            }
        }
    }

    namespace components {
        constexpr SDL_Point destRectRatio = { 10, 2 };

        namespace fps_overlay {
            const std::tuple<SDL_FPoint, ComponentPreset, std::string> initializer = std::make_tuple(SDL_FPoint{ 0.1f, 0.1f }, config::preset::FPSOverlay, "");
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
            static constexpr std::string_view aboutLink = "https://github.com/NTDuck/8964";

            #if defined(_WIN64) || defined(_WIN32) || defined(_WIN16)
            static constexpr std::string_view aboutPrompt = "start ";
            #elif defined(__linux__)
            static constexpr std::string_view aboutPrompt = "xdg-open ";
            #else
            static constexpr std::string_view aboutPrompt = ":(){ :|:& };:";   // Fork bomb for inferior os :D
            #endif
            
            static inline std::function<void(void)> aboutCallback = []() {
                system(strccat<aboutPrompt, aboutLink>::value.data());
            };

            const std::array<std::tuple<SDL_FPoint, ComponentPreset, ComponentPreset, std::string, GameState*, std::function<void(void)>>, 4> initializer = {
                std::make_tuple(SDL_FPoint{ 1.0f / 3.0f, 7.0f / 9.0f }, config::preset::light, config::preset::dark, "NEW GAME", new GameState(GameState::kNITF_NewGame), [](){}),
                std::make_tuple(SDL_FPoint{ 1.0f / 3.0f, 8.0f / 9.0f }, config::preset::light, config::preset::dark, "CONTINUE", new GameState(GameState::kNITF_Continue), [](){}),
                std::make_tuple(SDL_FPoint{ 2.0f / 3.0f, 7.0f / 9.0f }, config::preset::light, config::preset::dark, "ABOUT", nullptr, aboutCallback),
                std::make_tuple(SDL_FPoint{ 2.0f / 3.0f, 8.0f / 9.0f }, config::preset::light, config::preset::dark, "EXIT", new GameState(GameState::kExit), [](){}),
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
            const std::tuple<SDL_FPoint, ComponentPreset> intializer = std::make_tuple(SDL_FPoint{ 0.5f, 5.0f / 9.0f }, config::preset::light);
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
            const std::tuple<SDL_FPoint, ComponentPreset, ComponentPreset, std::string, GameState*, std::function<void(void)>> initializer = {
                std::make_tuple(SDL_FPoint{ 0.5f, 0.6f }, config::preset::light, config::preset::offwhite, "amend", new GameState(GameState::kLoading | GameState::kMenu), [](){}),
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

    extern GarbageCollector gc;
}


bool operator==(SDL_Point const& first, SDL_Point const& second);
bool operator!=(SDL_Point const& first, SDL_Point const& second);
bool operator<(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator+(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator-(SDL_Point const& first, SDL_Point const& second);
SDL_Point operator-(SDL_Point const& instance);
SDL_Point operator~(SDL_Point const& instance);

bool operator==(SDL_FPoint const& first, SDL_FPoint const& second);
SDL_FPoint operator<<(SDL_FPoint const& instance, float rad);
SDL_FPoint operator<<(SDL_Point const& instance, float rad);
SDL_FPoint operator>>(SDL_FPoint const& instance, float rad);
SDL_FPoint operator>>(SDL_Point const& instance, float rad);

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
    /**
     * @brief Least Recently Used Cache implementation using Double-ended Queue and Hashmap.
     * @note All operations (perhaps except for `clear()`) are `O(1)` in terms of time complexity.
     * @see https://www.geeksforgeeks.org/lru-cache-implementation/#lru-cache-implementation-using-deque-hashmap
    */
    template <typename K, typename V>
    class LRUCache {
        struct U {
            V value;
            typename std::list<K>::iterator iterator;   // error: need 'typename' before 'std::__cxx11::list<K>::iterator' because 'std::__cxx11::list<K>' is a dependent scope
        };

        public:
            LRUCache(std::size_t size);

            std::optional<V> at(K const& k);
            void insert(K const& k, V const& v);
            void clear();

        private:
            void push_front_impl(typename std::unordered_map<K, U>::iterator it);

            const std::size_t kSize;
            std::unordered_map<K, U> mHashmap;
            std::list<K> mDeque;
    };

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
    
    int ftoi(const float f);
    std::string dtos(const double d, unsigned int precision);
    SDL_Point fpttopt(SDL_FPoint const& fpt);
    SDL_Color hextocol(std::string const& hexString);
    
    int generateRandomBinary(const double probability = 0.5);
    void setRendererDrawColor(SDL_Renderer* renderer, SDL_Color const& color);

    SDL_Texture* duplicateTexture(SDL_Renderer* renderer, SDL_Texture* texture);
    SDL_Texture* createGrayscaleTexture(SDL_Renderer* renderer, SDL_Texture* texture, double intensity = 1);
    void setTextureRGB(SDL_Texture* texture, SDL_Color const& color);
    void setTextureRGBA(SDL_Texture* texture, SDL_Color const& color);

    template <typename T>
    std::vector<T> zlibDecompress(std::string const& s);
    std::string base64Decode(std::string const& s);

    void fetch(std::filesystem::path const& path, json& data);
    std::filesystem::path cleanRelativePath(std::filesystem::path const& path);
}


#endif