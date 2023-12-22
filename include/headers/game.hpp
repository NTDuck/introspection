#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>

#include <SDL.h>

#include <interface.hpp>
#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


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
        explicit Game(const GameFlag& flags, SDL_Rect windowDimension, const int frameRate, const std::string title);

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

        void handleWindowEvent(const SDL_Event& event);
        void handleMouseEvent(const SDL_Event& event);
        void handleKeyBoardEvent(const SDL_Event& event);

        /**
         * The pointer to the main window.
        */
        SDL_Window* window;
        /**
         * The pointer to the `SDL_Surface` bound to the main window.
        */
        SDL_Surface* windowSurface;
        /**
         * The ID of the main window. Used to determine certain interactions.
        */
        Uint32 windowID;

        IngameInterface* ingameInterface;
        MenuInterface* menuInterface;
        Player* player;

        /**
         * Flags used for initialization. Predominantly SDL-native.
        */
        const GameFlag flags;
        SDL_Rect windowDimension;
        const int frameRate;
        const std::string title;

        /**
         * The current game state. Is strictly bound to the main control flow.
        */
        GameState state;

        static Game* instance;
};


#endif