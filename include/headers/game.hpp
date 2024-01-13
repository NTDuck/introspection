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
 * @brief A singleton class that controls are ingame operations.
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

        void handleInterfaces();

        void handleEntities();
        void handleEntitiesMovement();
        void handleEntitiesInteraction();
        template <typename Active, typename Passive>
        void onEntityCollision(Active& active, Passive& passive);
        template <typename Active, typename Passive>
        void onEntityAnimation(AnimationType animationType, Active& active, Passive& passive);

        void handleWindowEvent(SDL_Event const& event);
        void handleMouseEvent(SDL_Event const& event);
        void handleKeyBoardEvent(SDL_Event const& event);

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

        SDL_Texture* textureEx = nullptr;
        // SDL_FPoint tileCountEx = { 0, 13.0f };
        // SDL_Rect srcRectEx;

        const GameInitFlag flags;
        SDL_Rect windowDimension;
        const int frameRate;
        const std::string title;
};


#endif