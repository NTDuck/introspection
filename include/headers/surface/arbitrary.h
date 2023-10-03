#pragma once

#include <SDL.h>
#include <SDL_image.h>


class ArbitraryBackgroundImage {
    public:
        ArbitraryBackgroundImage();
        ~ArbitraryBackgroundImage();

        SDL_Surface* surface;
        const char* path = "zassets/images/banner/cirnoday.bmp";   // get this right
};