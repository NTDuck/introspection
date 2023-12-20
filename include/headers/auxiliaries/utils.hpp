#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <vector>
#include <type_traits>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


bool operator==(const SDL_Point& first, const SDL_Point& second);
bool operator!=(const SDL_Point& first, const SDL_Point& second);
bool operator<(const SDL_Point& first, const SDL_Point& second);
SDL_Point operator+(const SDL_Point& first, const SDL_Point& second);
SDL_Point operator-(const SDL_Point& first, const SDL_Point& second);


namespace utils {
    template <class Base, class Derived>
    struct isDerivedFrom {
        static_assert(std::is_base_of<Base, Derived>::value, "`Derived` must derive from `Base`");
    };

    template <typename Iterable, typename Function, typename... Args>
    void iterate(const Iterable& iterable, Function&& function, Args&&... args);
    
    int convertFloatToInt(float f);
    int generateRandomBinary(const double probability = 0.5);
    double calculateDistance(const SDL_Point& first, const SDL_Point& second);
    std::string base64Decode(const std::string& s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string& s);

    void readJSON(const std::filesystem::path& path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(level::LevelMapping& mapping);
    void loadLevelData(level::LevelData& currentLevelData, const json& JSONLevelData);
    void loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, const json& jsonData);

    const tile::TilelayerTilesetData* getTilesetData(const tile::TilelayerTilesetData::Collection& tilesetDataCollection, int gid);
};


#endif