#include <iostream>

#include <meta.hpp>
#include <entities/player.hpp>

#include <auxiliaries/utils.hpp>


Player::Player(SDL_Point destCoords) : TextureWrapper::TextureWrapper(destCoords) {
    srcRect = {0, 0, 32, 32};   // arbitrary
}

Player::~Player() {
    TextureWrapper::~TextureWrapper();
}

void Player::init() {
    TextureWrapper::init("assets/graphics/sprites/player/hooded/hooded-full.png");
}

/**
 * @brief Handle player movement based on keyboard input.
*/
void Player::handleKeyboardEvent(const SDL_Event& event, TileCollection& tileCollection) {
    // Only move if the player is not already moving
    if (velocity.x || velocity.y) return;

    switch (event.key.keysym.sym) {
        case SDLK_w: --velocity.y; break;
        case SDLK_s: ++velocity.y; break;
        case SDLK_a: --velocity.x; flip = SDL_FLIP_HORIZONTAL; break;
        case SDLK_d: ++velocity.x; flip = SDL_FLIP_NONE; break;
    }

    // Check if the next `Tile` is valid
    SDL_Point nextDestCoords = destCoords;
    nextDestCoords.x += velocity.x;
    nextDestCoords.y += velocity.y;

    if (!Player::validateMove(nextDestCoords, tileCollection)) velocity = {0, 0};
}

/**
 * @brief Check whether moving the player from one `Tile` to the next is valid.
 * @warning The `entityWalkable` blindly determines walkability of all gids in a tileset. This method, therefore, is only a temporary, incomplete approach, and should be thoroughly re-assessed in future commits.
*/
bool Player::validateMove(SDL_Point nextDestCoords, const TileCollection& tileCollection) {
    // Check whether new tile exceeds the map limit
    if (nextDestCoords.x < 0 || nextDestCoords.y < 0 || nextDestCoords.x >= globals::TILE_DEST_COUNT.x || nextDestCoords.y >= globals::TILE_DEST_COUNT.y) return false;

    /**
     * Check whether the player should be "allowed" to move to the next `Tile` based on the following criteria:
     * 1. The next `Tile` contains at least one walkable type i.e. `entityWalkable = 0`
     * 2. The next `Tile` contains no obstructing type i.e. `entityWalkable = 2`
     * 3. The next `Tile` is "located lower" than the current `Tile` (pending)
    */
    bool isWalkable = false;

    for (const auto& gid : tileCollection[nextDestCoords.y][nextDestCoords.x]) {
        if (!gid) continue;
        int entityWalkable = std::stoi(utils::getTilesetData(gid).properties.find("entityWalkable") -> second);
        switch (entityWalkable) {
            case 0: isWalkable = true; break;
            case 1: break;
            case 2: return false;
        }
    }

    return isWalkable;
}