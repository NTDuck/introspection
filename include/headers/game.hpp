#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>

#include <SDL.h>

#include <interface.hpp>
#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief A singleton class that controls are ingame operations.
*/
class Game {
    public:
        static Game* instantiate(const GameFlag& flags, SDL_Rect windowDimension, const int frameRate, const std::string title);

        Game(const Game&) = delete;   // copy constructor
        Game& operator=(Game const&) = delete;   // copy assignment constructor
        Game(Game&&) = delete;   // move constructor
        Game& operator=(Game&&) = delete;   // move assignment constructor
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
        void onEntityAnimation(tile::AnimatedEntitiesTilesetData::AnimationType animationType, Active& active, Passive& passive);

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

        IngameInterface* interface;
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


template <>
void Game::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter);

template <>
void Game::onEntityCollision<Player, Slime>(Player& player, Slime& slime);


#endif