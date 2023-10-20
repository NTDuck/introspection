#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.h>


namespace globals {
    /**
     * @brief Load the tilesets.
     * @note Should be called before any rendering.
    */
    void init(SDL_Renderer* renderer) {
        const std::string base = "assets/graphics/sprites/";

        TILESET_MAPPING = {
            {TileSet::FLOOR_GRASS, {nullptr, base + "environments/floor-grass.png", 1, {8, 8}, {32, 32}}},
            {TileSet::WALL, {nullptr, base + "environments/wall.png", 65, {16, 16}, {32, 32}}},
            {TileSet::STRUCT, {nullptr, base + "environments/struct.png", 321, {16, 16}, {32, 32}}},
        };

        for (auto& pair : globals::TILESET_MAPPING) pair.second.texture = IMG_LoadTexture(renderer, pair.second.path.c_str());
    }

    /**
     * @brief Deallocate the tilesets.
     * @note Should be called when the program terminates.
    */
    void cleanup() {
        for (const auto& pair : globals::TILESET_MAPPING) SDL_DestroyTexture(pair.second.texture);
    }

    SDL_Point WINDOW_SIZE;
    SDL_Point TILE_SRC_SIZE;
    SDL_Point TILE_DEST_SIZE;
    SDL_Point TILE_DEST_COUNT;
    SDL_Point OFFSET;
    TilesetDataMapping TILESET_MAPPING;
}