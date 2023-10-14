#pragma once

#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <interface.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


/**
 * @brief The master class that controls all operations in-game.
*/
class Game {
    public:
        Game(Flags flags, SDL_Rect dims, const int frameRate, const std::string title);
        ~Game();

        void start();

    private:
        void init();
        void gameLoop();

        void blit();
        void render();

        void handleEvents();
        void handleWindowEvent(const SDL_Event* event);
        void handleMouseEvent(const SDL_Event* event);
        void handleKeyBoardEvent(const SDL_Event* event);

        // SDL2-native assets
        /**
         * @note For unknown reasons (or lack of brain), attempting to bind the renderer elsewhere yields weird error messages. Therefore, related functions and objects will now access the renderer as a parameter.
         * @see https://stackoverflow.com/questions/12506979/what-is-the-point-of-an-sdl2-texture
        */
        SDL_Renderer* renderer = nullptr;

        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;
        Uint32 windowID;

        /**
         * Member instances.
        */
        Interface interface;

        // Initialization parameters
        const Flags flags;
        SDL_Rect dims;
        const int frameRate;
        const std::string title;

        GameState state;
};