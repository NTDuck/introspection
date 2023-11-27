#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    int generateRandomBinary();
    std::string base64Decode(const std::string& s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string& s);

    void readJSON(const std::filesystem::path& path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(level::LevelMapping& mapping);
    void loadLevelData(level::LevelData& currentLevelData, const json& JSONLevelData);
    void loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, const json& jsonData);

    tile::TilelayerTilesetData getTilesetData(tile::TilelayerTilesetData::Collection& tilesetDataCollection, int gid);
};


#endif