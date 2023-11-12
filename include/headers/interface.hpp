#pragma once

#include <unordered_map>

#include <SDL.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


class Interface {
    public:
        Interface(std::string levelName);
        ~Interface();
        
        void init();
        void render();

        void changeLevel(std::string levelName);
        void onLevelChange();
        void onWindowChange();

    private:
        void loadLevel();
        void renderBackgroundToTexture();
        void renderLevelTilesToTexture();

        std::string levelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        LevelMapping levelMapping;

        /**
         * @brief A temporary storage that is rendered every frame. Used to prevent multiple unnecessary calls of `SDL_RenderCopy()`.
         * @note Needs optimization to perfect relative positions of props to entities.
        */
        SDL_Texture* texture = nullptr;
};