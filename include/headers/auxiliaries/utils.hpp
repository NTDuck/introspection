#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    /**
     * @brief Define functors that provide hash functions for data structures whose hash function could be non-existent or deficient.
    */
    namespace hashers {
        struct SDL_Point_Hasher {
            std::size_t operator()(const SDL_Point& obj) const;
        };
    };

    /**
     * @brief Define functors that provide operators for data structures whose operators could be non-existent or deficient.
    */
    namespace operators {
        struct SDL_Point_Equality_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const;
        };

        struct SDL_Point_Less_Than_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const;
        };
    };

    std::string base64Decode(const std::string& s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string& s);

    void readJSON(const std::filesystem::path& path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(level::LevelMapping& mapping);
    void loadLevelData(level::LevelData& currentLevelData, const json& data);
    void loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, const json& jsonData);

    tile::TilelayerTilesetData getTilesetData(tile::TilelayerTilesetData::Collection& tilesetDataCollection, int gid);
};


#endif