#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <SDL.h>


/**
 * @brief Represents SDL flags and other configurations used in subsystem initialization.
 * @note Multiple flags are combinable via bitwise OR `|`.
*/
struct Flags {
    Uint32 init;
    Uint32 window;
    Uint32 renderer;   // try SDL_RENDERER_PRESENTVSYNC
    int image;
    std::unordered_map<std::string, std::string> hints;
};

/**
 * @brief Encapsulates data related to a tile.
 * 
 * @param textures 
*/
struct TileData {
    std::vector<SDL_Texture*> textures;
    std::vector<SDL_Rect> srcRects;
    SDL_Rect destRect;
    double angle;
    SDL_RendererFlip flip;
};