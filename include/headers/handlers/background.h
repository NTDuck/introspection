#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


class BackgroundHandler {
    public:
        BackgroundHandler(Dimensions dimensions);
        ~BackgroundHandler();

        void changeBackground(SDL_Surface* windowSurface, BackgroundType type, SDL_PixelFormat* fmt);

        SDL_Surface* background;
        unsigned char position;

    private:
        SDL_Rect stretchRect;
};