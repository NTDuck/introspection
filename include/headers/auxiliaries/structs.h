#pragma once

#include <unordered_map>
#include <string>

#include <SDL.h>
#include <SDL_image.h>


// multiple flags combinable via bitwise OR
struct Flags {
    Uint32 init;
    Uint32 window;
    Uint32 renderer;   // try SDL_RENDERER_PRESENTVSYNC
    int image;
    std::unordered_map<std::string, std::string> hints;
};

struct Dimensions {
    // denotes central not upper-left
    int _x;
    int _y;
    unsigned short int _w;
    unsigned short int _h;
};

struct Mouse {
    Uint32 state;
    int _x;
    int _y;
};