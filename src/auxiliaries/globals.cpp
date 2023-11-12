#include <SDL.h>
#include <auxiliaries/globals.hpp>


namespace globals {
    /**
     * @brief Deallocate the tilesets.
     * @note Should be called when the program terminates.
    */
    void dealloc() {
        if (globals::renderer != nullptr) SDL_DestroyRenderer(globals::renderer);
        for (const auto& data : globals::TILESET_COLLECTION) if (data.texture != nullptr) SDL_DestroyTexture(data.texture);
    }

    SDL_Renderer* renderer = nullptr;
    SDL_Point WINDOW_SIZE;
    SDL_Point TILE_SRC_SIZE;
    SDL_Point TILE_DEST_SIZE;
    SDL_Point TILE_DEST_COUNT;
    SDL_Point OFFSET;
    TilesetDataCollection TILESET_COLLECTION;
    globals::levelData::Level currentLevel;
}