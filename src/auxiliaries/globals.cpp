#include <SDL.h>
#include <auxiliaries/globals.hpp>


/**
 * @note Default `NULL` value.
*/
TilesetData::TilesetData() : texture(nullptr), firstGID(0), srcCount({0, 0}), srcSize({0, 0}), properties({}), propertiesEx({}) {}

/**
 * @brief Clean up resources to prevent memory leaks.
*/
void TilesetData::dealloc() {
    if (texture != nullptr) SDL_DestroyTexture(texture);
    *this = TilesetData();   // Simulate `NULL`
}

/**
 * @brief The global namespace. Member variables accessible to all classes.
*/
namespace globals {
    /**
     * @brief Provide required data for level-loading.
    */
    namespace leveldata {
        std::size_t TextureData::TextureData_Hasher::operator()(const TextureData& obj) const { return std::hash<int>()(obj.destCoords.x) ^ (std::hash<int>()(obj.destCoords.y) << 1); }

        bool TextureData::TextureData_Equality_Operator::operator()(const TextureData& first, const TextureData& second) const { return first.destCoords.x == second.destCoords.x && first.destCoords.y == second.destCoords.y; }

        bool TextureData::TextureData_Less_Than_Operator::operator()(const TextureData& first, const TextureData& second) const { return (first.destCoords.y < second.destCoords.y) || (first.destCoords.y == second.destCoords.y && first.destCoords.x < second.destCoords.x); }
    }

    /**
     * @brief Deallocate the tilesets.
     * @note Should be called when the program terminates.
    */
    void dealloc() {
        if (globals::renderer != nullptr) SDL_DestroyRenderer(globals::renderer);
        for (auto& tilesetData : globals::TILESET_COLLECTION) tilesetData.dealloc();
    }

    SDL_Renderer* renderer = nullptr;
    SDL_Point WINDOW_SIZE;
    SDL_Point TILE_SRC_SIZE;
    SDL_Point TILE_DEST_SIZE;
    SDL_Point TILE_DEST_COUNT;
    SDL_Point OFFSET;
    TilesetDataCollection TILESET_COLLECTION;
    globals::leveldata::LevelData currentLevelData;
}