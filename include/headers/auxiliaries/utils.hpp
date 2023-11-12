#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include <auxiliaries/globals.hpp>


namespace utils {
    /**
     * @brief Define/re-define hash function objects for needy classes.
     * @note Required for `std::unordered_set` and `std::unordered_map`.
     * @bug `std::bad_alloc` temporarily patched with `noexcept`.
    */
    namespace hashers {
        struct SDL_Point_Hasher {
            std::size_t operator()(const SDL_Point& obj) const noexcept {
                return std::hash<int>()(obj.x) ^ (std::hash<int>()(obj.y) << 1);
            }
        };
    };

    /**
     * @brief Define/re-define common operators needed for `std::set`, `std::map`, `std::unordered_set`, `std::unordered_map`.
    */
    namespace operators {
        struct SDL_Point_Equality_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const {
                return first.x == second.x && first.y == second.y;
            }
        };

        struct SDL_Point_Less_Than_Operator {
            bool operator()(const SDL_Point& first, const SDL_Point& second) const {
                return (first.y < second.y) || (first.y == second.y && first.x < second.x);
            }
        };
    };

    std::string base64Decode(const std::string s);

    template <typename T>
    std::vector<T> zlibDecompress(const std::string s);

    void readJSON(const std::filesystem::path path, json& data);
    void cleanRelativePath(std::filesystem::path& path);
    SDL_Color SDL_ColorFromHexString(const std::string& hexString);

    void loadLevelsData(LevelMapping& mapping);
    void loadLevelData(globals::levelData::Level& levelData, const json& data);
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData);

    TilesetData getTilesetData(int gid);
}