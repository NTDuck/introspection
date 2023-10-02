#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <managers/window.h>


// multiple flags combinable via bitwise OR
struct Flags {
    Uint32 init = SDL_INIT_EVERYTHING;
    Uint32 renderer = 0;   // try SDL_RENDERER_PRESENTVSYNC
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
        void handleMouseEvent(SDL_Event* event);
        void handleKeyBoardEvent(SDL_Event* event);

        WindowManager windowManager = WindowManager();

        SDL_Renderer* renderer = nullptr;

        Flags flags;
        Mouse mouse;
        State gamestate = State::PLAYING;

        unsigned short int frameRate = 60;   // per second; mutability intended
};