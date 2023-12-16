#ifndef INTERFACE_H
#define INTERFACE_H

#include <unordered_map>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Represent the in-game interface.
*/
class IngameInterface : public Singleton<IngameInterface> {
    friend Singleton<IngameInterface>;
    public:
        using Singleton<IngameInterface>::instantiate;

        ~IngameInterface();

        static void initialize();
        static void deinitialize();
        
        void render();

        void changeLevel(const level::LevelName levelName);
        void onLevelChange();
        void onWindowChange();

    private:
        IngameInterface(const level::LevelName levelName);

        void loadLevel();
        void renderBackground();
        void renderLevelTiles();

        level::LevelName levelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        static level::LevelMapping levelMapping;

        /**
         * @brief A temporary storage that is rendered every frame. Used to prevent multiple unnecessary calls of `SDL_RenderCopy()`.
         * @note Needs optimization to perfect relative positions of props to entities.
        */
        SDL_Texture* texture;
};


#endif