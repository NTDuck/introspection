#pragma once

#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    std::string base64Decode(const std::string s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string s);

    void readJSON(const std::string path, json& data);
    void cleanRelativePath(std::string& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(const std::string xmlpath, LevelMapping& mapping);
    void loadLevelData(Level& levelData, const json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);

    TilesetData getTilesetData(int gid);
}