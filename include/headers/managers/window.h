#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <surface/arbitrary.h>


struct Dimensions {
    // denotes central not upper-left
    int _x = SDL_WINDOWPOS_CENTERED;
    int _y = SDL_WINDOWPOS_CENTERED;
    unsigned short int _w, _h;
};

class WindowManager {
    public:
        WindowManager();
        ~WindowManager();

        void init();
        void setWindowSurface();
        void handleWindowEvent(SDL_Event* event);

        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;
        Uint32 windowID;

        Uint32 flags = SDL_WINDOW_SHOWN;
        Dimensions dimensions;

        ArbitraryBackgroundImage background;

        const char* title;
};