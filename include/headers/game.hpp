#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_set>
#include <unordered_map>

#include <SDL.h>

#include <timers.hpp>
#include <interface.hpp>
#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Controls everything.
 * @note Dependencies could communicate via `globals::gameState`.
*/
class Game final : public Singleton<Game> {
    friend Singleton<Game>;   // Required for private constructor
    public:
        INCL_SINGLETON(Game)

        ~Game();
        void start();

    private:
        Game(GameInitFlag const& flags, SDL_Rect windowDimension, const int frameRate, const std::string title);

        void initialize();
        void startGameLoop();

        void handleDependencies();
        void handleEvents();
        void render() const;

        void onLevelChange();
        void onWindowChange();

        void handleWindowEvent(SDL_Event const& event);
        void handleKeyBoardEvent(SDL_Event const& event) const;
        void handleMouseEvent(SDL_Event const& event) const;
        void handleCustomEventGET(SDL_Event const& event) const;

        /**
         * The pointer to the main window.
        */
        SDL_Window* window = nullptr;

        /**
         * The pointer to the `SDL_Surface` bound to the main window.
        */
        SDL_Surface* windowSurface = nullptr;

        /**
         * The ID of the main window. Used to determine certain interactions.
        */
        Uint32 windowID;

        const GameInitFlag flags;
        SDL_Rect windowDimension;
        const int frameRate;
        const std::string windowTitle;
};


#endif