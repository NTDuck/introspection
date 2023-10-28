#pragma once

#include <unordered_map>

#include <SDL.h>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


class Interface {
    public:
        Interface(LevelState level);
        ~Interface();
        
        void init();

        void blit();
        void render();

        void changeLevel(LevelState level);

    private:
        void setupLevelMapping();

        void renderBackground();
        void renderLevel();

        void loadLevel();

        LevelState level;

        // Containing all level-loading methods.
        std::unordered_map<LevelState, std::string> levelMapping;
        // Simulate all tiles presented on the window. Usable as a mapping.
        SDL_Texture* texture = nullptr;
};