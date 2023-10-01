#pragma once

#include <iostream>
#include "../sdl2/SDL.h"
#include "../sdl2/SDL_image.h"


enum GameState {PLAY, EXIT};


class Game {
    public:
        // constructor & destructor
        Game();
        ~Game();
        void run();

    private:
        void init(const char* title, int _x, int _y, int w, int h, Uint32 flags);
        void gameLoop();
        void handleEvents();

        SDL_Window* window;
        SDL_Renderer* renderer;

        int screenWidth;
        int screenHeight;

        GameState gamestate;
};