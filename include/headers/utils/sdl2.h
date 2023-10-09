#pragma once

#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/structs.h>


namespace utils {
    std::vector<SDL_Rect> createRects(TextureWrapperInitData data);
    bool isPermanent(SpriteState state);
}