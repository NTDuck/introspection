#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <SDL.h>

#include <auxiliaries/defs.h>


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
 * @brief Represents a pair of integers.
*/
struct Dat2u {
    int _x;
    int _y;
};

/**
 * @brief Represents a pair of non-negative integers.
*/
struct Dat2hu {
    unsigned short int _x;
    unsigned short int _y;
};

/**
 * @brief Represents the mouse's state and position.
 * @see Game::handleMouseEvent()
*/
struct Mouse {
    Uint32 state;
    Dat2u pos;
};

/**
 * @brief Encapsulates data required for `TextureWrapper` initialization.
 * 
 * @param state the affiliated `SpriteState` used to determine rendered sprites.
 * @param path a path from the filesystem to load the spreadsheet from.
 * @param sheetSize the size, in pixels, of the spritesheet.
 * @param sheetDivs the number of frames on the sprite sheet.
 * @param spriteStart the index of the starting sprite.
 * @param spriteCounts the number of sprites to rotate.
 * 
 * @see TextureWrapper::spriteMapping
 * @see utils::createFrameRects()
*/
struct TextureWrapperInitData {
    SpriteState state;
    SDL_Texture* texture;
    Dat2u sheetSize;
    Dat2hu sheetDivs;
    Dat2hu spriteStart;
    unsigned char spriteCounts;

    bool operator<(const TextureWrapperInitData& other) const {
        // Define a comparison logic for TextureWrapperInitData
        // You can use any criteria for comparison that makes sense for your application.
        // For example, comparing based on 'state' or 'texture' member variables.
        // Here, we'll compare based on 'state'.
        return state < other.state;
    }

};

/**
 * @brief Encapsulates data required for `TextureWrapper.spriteMapping`.
 * 
 * @param texture usually the entire spritesheet.
 * @param rects represents the compartmentalized portion of the spritesheet used for rendering.
*/
struct TextureWrapperMappingData {
    SDL_Texture* texture;
    std::vector<SDL_Rect> rects;
};