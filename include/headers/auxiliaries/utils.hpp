#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    std::string base64Decode(const std::string s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string s);

    void readJSON(const std::filesystem::path path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(LevelMapping& mapping);
    void loadLevelData(Level& levelData, const json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);

    TilesetData getTilesetData(int gid);
}