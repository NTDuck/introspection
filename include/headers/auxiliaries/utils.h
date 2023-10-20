#pragma once

#include <array>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.h>


namespace utils {
    void readJSON(const std::string path, json& data);
    void parseConfigData(const std::string path, Flags& flags, SDL_Rect& dims, int& frameRate);
}