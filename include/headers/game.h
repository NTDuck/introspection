#pragma once

#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <characters/player.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


/**
 * @brief The master class that controls all operations in-game.
*/
class Game {
    public:
        Game(Flags flags, Dat2u pos, Dat2hu size, unsigned short int frameRate, std::string title);
        ~Game();

        void start();

    private:
        void init();
        void gameLoop();

        void render();

        void handleEvents();
        void handleWindowEvent(const SDL_Event* event);
        void handleMouseEvent(const SDL_Event* event);
        void handleKeyBoardEvent(const SDL_Event* event);

        void changeWindowSurface();
        void changeBackground();

        /**
         * @note For unknown reasons (or lack of brain), attempting to bind the renderer elsewhere yields weird error messages. Therefore, related functions and objects will now have access the renderer as a parameter.
         * @see https://stackoverflow.com/questions/12506979/what-is-the-point-of-an-sdl2-texture
        */
        SDL_Renderer* renderer = nullptr;

        // SDL assets
        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;

        Uint32 windowID;

        SDL_Texture* backgroundTexture;
        Player player;

        // misc
        const Flags flags;
        Dat2u pos;
        Dat2hu size;
        Mouse mouse;
        GameState state = GameState::MENU;

        unsigned short int frameRate;
        std::string title;
};