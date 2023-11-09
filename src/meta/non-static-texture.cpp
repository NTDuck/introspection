#include <filesystem>
#include <sstream>

#include <SDL.h>
#include <pugixml/pugixml.hpp>

#include <meta.hpp>
#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>

NonStaticTextureWrapper::NonStaticTextureWrapper() {}

NonStaticTextureWrapper::~NonStaticTextureWrapper() {
    BaseTextureWrapper::~BaseTextureWrapper();
}

/**
 * @brief Initialize the player and populate `rotatingGIDs` for srcRect rotation.
 * @see https://en.cppreference.com/w/cpp/utility/from_chars (hopefully better than `std::istringstream`)
*/
void NonStaticTextureWrapper::initAbstract(std::filesystem::path xmlPath) {
    if (!std::filesystem::exists(xmlPath)) return;
    BaseTextureWrapper::initAbstract(xmlPath);

    for (const auto& pair : properties) {
        std::istringstream iss(pair.second);
        std::pair<int, int> GIDs;
        iss >> GIDs.first >> GIDs.second;
        rotatingGIDs[pair.first] = GIDs;
    }

    // This is really ugly and needs immediate correction
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(xmlPath.c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    srcRectCount.x = document.child("tileset").attribute("columns").as_int();
    srcRectCount.y = document.child("tileset").attribute("tilecount").as_int() / srcRectCount.x;

    currAnimationState = "animation-walk";
    animationUpdateRate = config::ANIMATION_UPDATE_RATE_NONSTATIC_TEXTURE;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}

void NonStaticTextureWrapper::move() {
    BaseTextureWrapper::move();
    updateAnimation();
}

/**
 * @brief Check whether moving the texture from one `Tile` to the next is valid.
 * @note The sixth commandment: If a function be advertised to return an error code in the event of difficulties, thou shalt check for that code, yea, even though the checks triple the size of thy code and produce aches in thy typing fingers, for if thou thinkest `it cannot happen to me`, the gods shall surely punish thee for thy arrogance.
*/
bool NonStaticTextureWrapper::validateMove() {
    if (!BaseTextureWrapper::validateMove()) return false;

    // Find the collision-tagged tileset associated with `gid`
    auto findCollisionLevelGID = [&](const SDL_Point& coords) {
        static TilesetData* tilesetData = nullptr;
        for (const auto& gid : globals::currentLevel.tileCollection[coords.y][coords.x]) {
            tilesetData = new TilesetData(utils::getTilesetData(gid));
            if (!gid && tilesetData -> properties["collision"] != "true") continue;
            // if (!collisionTransitionLevel) collisionTransitionLevel = std::stoi(tilesetData -> properties["collision-transition"]) + tilesetData -> firstGID;
            return gid;
        }
        return 0;
    };

    int currCollisionLevel = findCollisionLevelGID(destCoords);
    int nextCollisionLevel = findCollisionLevelGID(*nextDestCoords);

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
 * @brief Switch from one sprite to the next.
 * @see <src/interface.cpp> Interface.renderLevel() (classmethod)
*/
void NonStaticTextureWrapper::updateAnimation() {
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
void NonStaticTextureWrapper::resetAnimation(const std::string nextAnimationState) {
    currAnimationState = nextAnimationState;
    currAnimationGID = rotatingGIDs[currAnimationState].first;
}

void NonStaticTextureWrapper::onMoveStart() {
    resetAnimation("animation-walk");
}

void NonStaticTextureWrapper::onMoveEnd() {
    BaseTextureWrapper::onMoveEnd();
    resetAnimation("animation-idle");
}