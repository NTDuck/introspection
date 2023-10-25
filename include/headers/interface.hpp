#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


class Interface {
    public:
        Interface(Level level);
        ~Interface();
        
        void init();

        void blit();
        void render();

        void changeLevel(Level level);

        TileCollection tileCollection;
        
    private:
        void setupLevelMapping();

        void renderBackground();
        void renderLevel();

        void loadLevel();

        Level level;

        // Containing all level-loading methods.
        std::unordered_map<Level, std::string> levelMapping;
        // Simulate all tiles presented on the window. Usable as a mapping.
        SDL_Texture* texture = nullptr;
};