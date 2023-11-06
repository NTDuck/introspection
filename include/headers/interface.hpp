#pragma once

#include <unordered_map>

#include <SDL.h>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


class Interface {
    public:
        Interface(std::string levelName);
        ~Interface();
        
        void init();

        void blit();
        void render();

        void changeLevel(std::string levelName);

    private:
        void renderBackground();
        void renderLevel();

        void loadLevel();

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