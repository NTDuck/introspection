#include <filesystem>
#include <sstream>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <entities/player.hpp>
#include <auxiliaries/utils.hpp>


Player::Player() {}

Player::~Player() {
    TextureWrapper::~TextureWrapper();
}

/**
 * @brief Check whether moving the player from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
*/
bool validateMove(const SDL_Point& currDestCoords, const SDL_Point& nextDestCoords, const TileCollection& tileCollection) {
    // Check whether new tile exceeds the map limit
    if (nextDestCoords.x < 0 || nextDestCoords.y < 0 || nextDestCoords.x >= globals::TILE_DEST_COUNT.x || nextDestCoords.y >= globals::TILE_DEST_COUNT.y) return false;

    // Find the collision-tagged tileset associated with `gid`
    // static int collisionTransitionLevel = 0;

    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        static TilesetData* tilesetData = nullptr;
        for (const auto& gid : tileCollection[coords.y][coords.x]) {
            tilesetData = new TilesetData(utils::getTilesetData(gid));
            if (!gid && tilesetData -> properties["collision"] != "true") continue;
            // if (!collisionTransitionLevel) collisionTransitionLevel = std::stoi(tilesetData -> properties["collision-transition"]) + tilesetData -> firstGID;
            return gid;
        }
        return 0;
    };

    int currCollisionLevel = findCollisionLevelGID(currDestCoords);
    int nextCollisionLevel = findCollisionLevelGID(nextDestCoords);

    /**
     * @details The move is validated only if `nextCollisionLevel` exists, and one of the following scenarios occurs:
     * 1. `currCollisionLevel` is equal to `collisionTransitionLevel`
     * 2. `nextCollisionLevel` is equal to `collisionTransitionLevel`
     * 3. `currCollisionLevel` is equal to `nextCollisionLevel`
    */
    if (!nextCollisionLevel) return false;
    // return (currCollisionLevel == collisionTransitionLevel || nextCollisionLevel == collisionTransitionLevel || currCollisionLevel == nextCollisionLevel);
    return currCollisionLevel == nextCollisionLevel;
}

/**
 * @brief Initialize the player and populate `rotatingGIDs` for srcRect rotation.
 * @see https://en.cppreference.com/w/cpp/utility/from_chars (hopefully better than `std::istringstream`)
*/
void Player::init() {
    if (!std::filesystem::exists(config::PLAYER_TILESET_PATH)) return;
    TextureWrapper::init(config::PLAYER_TILESET_PATH.string());

    for (const auto& pair : properties) {
        std::istringstream iss(pair.second);
        std::pair<int, int> GIDs;
        iss >> GIDs.first >> GIDs.second;
        rotatingGIDs[pair.first] = GIDs;
    }

    // This is really ugly and needs immediate correction
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(config::PLAYER_TILESET_PATH.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    srcRectCount.x = document.child("tileset").attribute("columns").as_int();
    srcRectCount.y = document.child("tileset").attribute("tilecount").as_int() / srcRectCount.x;

    currAnimationState = config::ANIMATION_PLAYER;
    animationUpdateRate = config::ANIMATION_PLAYER_UPDATE_RATE;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}

/**
 * @brief Handle player movement based on keyboard input.
*/
void Player::handleKeyboardEvent(const SDL_Event& event) {
    // Only move if the player is not already moving
    if (nextDestCoords != nullptr) return;

    // if ((nextDestCoords -> x < 0 || nextDestCoords -> y < 0 || nextDestCoords -> x >= globals::TILE_DEST_COUNT.x || nextDestCoords -> y >= globals::TILE_DEST_COUNT.y)) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: --velocity.y; break;
        case SDLK_s: ++velocity.y; break;
        case SDLK_a: --velocity.x; flip = SDL_FLIP_HORIZONTAL; break;
        case SDLK_d: ++velocity.x; flip = SDL_FLIP_NONE; break;
        default: return;
    }

    nextDestCoords = new SDL_Point({destCoords.x + velocity.x, destCoords.y + velocity.y});
    nextDestRect = new SDL_Rect(getDestRectFromCoords(*nextDestCoords));

    // Check if the next `Tile` is valid
    if (validateMove(destCoords, *nextDestCoords, globals::currentLevel.tileCollection)) onMoveStart(); else onMoveEnd();
}

/**
 * @brief Render the current sprite.
 * @note Override base method to enlarge player without affecting `srcCoords`, which is used in many operations.
*/
void Player::render() {
    SDL_Rect* _destRect = new SDL_Rect(destRect);
    _destRect -> x -= globals::TILE_DEST_SIZE.x * 0.5;
    _destRect -> y -= globals::TILE_DEST_SIZE.y * 1.5;
    _destRect -> w *= 2;
    _destRect -> h *= 2;
    SDL_RenderCopyEx(globals::renderer, texture, &srcRect, _destRect, angle, center, flip);
}

/**
 * @brief Switch from one sprite to the next.
 * @see <src/interface.cpp> Interface.renderLevel() (classmethod)
*/
void Player::updateAnimation() {
    static int animationUpdateCount = 0;
    ++animationUpdateCount;

    if (animationUpdateCount == animationUpdateRate) {
        animationUpdateCount = 0;
        if (currAnimationGID == rotatingGIDs[currAnimationState].second) currAnimationGID = rotatingGIDs[currAnimationState].first; else ++currAnimationGID;
    }

    srcRect.x = currAnimationGID % srcRectCount.x * srcRect.w;
    srcRect.y = currAnimationGID / srcRectCount.x * srcRect.h;
}

/**
 * @brief Reset the animation state. Should be called whenever switches to new animation state.
*/
void Player::resetAnimation(const std::string nextAnimationState) {
    currAnimationState = nextAnimationState;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}

/**
 * @brief Move the player and also switch from one sprite to the next.
*/
void Player::move() {
    TextureWrapper::move();
    updateAnimation();
}

void Player::onMoveStart() {
    resetAnimation("animation-walk");
}

void Player::onMoveEnd() {
    TextureWrapper::onMoveEnd();
    resetAnimation("animation-idle");
}