#pragma once

#include <iostream>

#include "../sdl2/SDL.h"
#include "../sdl2/SDL_image.h"


// multiple flags combinable via bitwise OR
struct Flags {
    Uint32 init = SDL_INIT_EVERYTHING;
    Uint32 window = SDL_WINDOW_SHOWN;
    Uint32 renderer = 0;   // try SDL_RENDERER_PRESENTVSYNC
};

struct Dimensions {
    // denotes central not upper-left
    int _x = SDL_WINDOWPOS_CENTERED;
    int _y = SDL_WINDOWPOS_CENTERED;
    unsigned short int _w, _h;
};

struct Mouse {
    Uint32 state;
    int _x, _y;
};

enum State {PLAYING, PAUSED, CUTSCENE, EXIT};


class Game {
    public:
        Game();
        ~Game();
        void run();

    private:
        void init();
        void gameLoop();
        void handleEvents();
        void handleWindowEvent(SDL_Event* event);
        void handleMouseEvent(SDL_Event* event);
        void handleKeyBoardEvent(SDL_Event* event);

        SDL_Window* window = nullptr;
        Uint32 windowID;
        SDL_Renderer* renderer = nullptr;

        Flags flags;
        Dimensions dimensions;
        Mouse mouse;
        State gamestate = State::PLAYING;


        const char* title;
        unsigned short int frameRate = 120;   // per second; mutability intended
};