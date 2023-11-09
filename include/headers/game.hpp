#pragma once

#include <string>

#include <SDL.h>

#include <interface.hpp>
#include <entities.hpp>
#include <auxiliaries/globals.hpp>


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
        void handleWindowEvent(const SDL_Event& event);
        void handleMouseEvent(const SDL_Event& event);
        void handleKeyBoardEvent(const SDL_Event& event);

        // SDL2-native assets
        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;
        Uint32 windowID;

        /**
         * Member instances.
        */
        Interface interface;
        Player player;

        // Initialization parameters
        const Flags flags;
        SDL_Rect dims;
        const int frameRate;
        const std::string title;

        GameState state;
};