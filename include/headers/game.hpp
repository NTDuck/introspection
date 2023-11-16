#pragma once

#include <string>
#include <unordered_map>

#include <SDL.h>

#include <interface.hpp>
#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief The master class that controls all operations in-game.
*/
class Game {
    public:
        Game(InitFlags flags, SDL_Rect dims, const int frameRate, const std::string title);
        ~Game();

        void start();

    private:
        void init();
        void gameLoop();

        void render();
        void handleMotion();

        void onLevelChange();
        void onWindowChange();

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
        Teleporter::Teleporters teleporters;

        // Initialization parameters
        const InitFlags flags;
        SDL_Rect dims;
        const int frameRate;
        const std::string title;

        GameState state;
};