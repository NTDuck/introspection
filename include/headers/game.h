#pragma once

#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <handlers/window.h>
#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


class Game {
    public:
        Game(Flags flags, Dimensions dimensions, unsigned short int frameRate, std::string title);
        ~Game();
        void start();

    private:
        void init();
        void gameLoop();

        void handleEvents();
        void handleMouseEvent(SDL_Event* event);
        void handleKeyBoardEvent(SDL_Event* event);

        WindowHandler windowHandler;
        BackgroundHandler& backgroundHandler;

        const Flags flags;
        Dimensions dimensions;
        Mouse mouse;
        GameState gameState;

        unsigned short int frameRate;   // per second; mutability intended
        std::string title;
};