#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


class BackgroundHandler {
    public:
        BackgroundHandler(Dimensions dimensions);
        ~BackgroundHandler();

        void changeBackground(SDL_Renderer* renderer, BackgroundType type);
        
        SDL_Texture* background;   // https://stackoverflow.com/questions/12506979/what-is-the-point-of-an-sdl2-texture
        unsigned char position;
};