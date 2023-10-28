#pragma once

#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    void readJSON(const std::string path, json& data);
    void loadLevelData(Level& levelData, json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);
    TilesetData getTilesetData(int gid);
}