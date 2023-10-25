#pragma once

#include <array>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    void readJSON(const std::string path, json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);
    TilesetData getTilesetData(int gid);
}