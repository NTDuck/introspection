#include <SDL.h>

#include <meta.hpp>
#include <entities/player.hpp>

#include <auxiliaries/utils.hpp>


Player::Player() {}

Player::~Player() {
    TextureWrapper::~TextureWrapper();
}

void Player::init() {
    TextureWrapper::init("assets/.tiled/hooded-full.tsx");
}

/**
 * @brief Check whether moving the player from one `Tile` to the next is valid.
 * @warning The `entityWalkable` blindly determines walkability of all gids in a tileset. This method, therefore, is only a temporary, incomplete approach, and should be thoroughly re-assessed in future commits.
*/
bool validateMove(const SDL_Point& currDestCoords, const SDL_Point& nextDestCoords, const TileCollection& tileCollection) {
    // Check whether new tile exceeds the map limit
    if (nextDestCoords.x < 0 || nextDestCoords.y < 0 || nextDestCoords.x >= globals::TILE_DEST_COUNT.x || nextDestCoords.y >= globals::TILE_DEST_COUNT.y) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point coords) {
        for (const auto& gid : tileCollection[coords.y][coords.x]) if (gid && utils::getTilesetData(gid).properties["collision"] == "true") return gid;
        return 0;
    };

    // Check whether the next tile is considered "walkable"
    int currCollisionLevel = findCollisionLevelGID(currDestCoords);
    int nextCollisionLevel = findCollisionLevelGID(nextDestCoords);

    if (!nextCollisionLevel || currCollisionLevel < nextCollisionLevel) return false;

    return true;
}

/**
 * @brief Handle player movement based on keyboard input.
*/
void Player::handleKeyboardEvent(const SDL_Event& event) {
    // Only move if the player is not already moving
    if (event.key.type != SDL_KEYDOWN || nextDestCoords != nullptr) return;   // Apparently `event.key.repeat` has to be there else the program magically terminates itself.

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
    if (!validateMove(destCoords, *nextDestCoords, globals::LEVEL.tileCollection)) onMoveEnd();
}