#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    namespace hashers {
        struct SDL_Point_Hasher {
            std::size_t operator()(const SDL_Point& obj) const;
        };
    };


    namespace operators {
        struct SDL_Point_Equality_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const;
        };

        struct SDL_Point_Less_Than_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const;
        };
    };

    std::string base64Decode(const std::string s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string s);

    void readJSON(const std::filesystem::path path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(LevelMapping& mapping);
    void loadLevelData(globals::leveldata::LevelData& levelData, const json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetData& tilesetData, const std::filesystem::path xmlPath);
    void loadTilesetData(SDL_Renderer* renderer, TilesetData& tilesetData, const json& tileset);
    void loadTilesetsData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);

    TilesetData getTilesetData(int gid);
}