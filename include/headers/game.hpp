#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>

#include <SDL.h>

#include <interface.hpp>
#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief A singleton class that controls are ingame operations.
*/
class Game final : public Singleton<Game> {
    friend Singleton<Game>;
    public:
        using Singleton<Game>::instantiate;

        ~Game();
        void start();

    private:
        explicit Game(GameInitFlag const& flags, SDL_Rect windowDimension, const int frameRate, const std::string title);

        void initialize();
        void startGameLoop();

        void handleEntities();
        void handleEvents();
        void render();

        void onLevelChange();
        void onWindowChange();

        void handleEntitiesMovement();
        void handleEntitiesInteraction();
        template <class Active, class Passive>
        void onEntityCollision(Active& active, Passive& passive);
        template <class Active, class Passive>
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

        IngameInterface* ingameInterface = nullptr;
        MenuInterface* menuInterface = nullptr;
        Player* player = nullptr;

        const GameInitFlag flags;
        SDL_Rect windowDimension;
        const int frameRate;
        const std::string title;

        /**
         * The current game state. Is strictly bound to the main control flow.
        */
        GameState state = GameState::kMenu;

        static Game* instance;
};


#endif