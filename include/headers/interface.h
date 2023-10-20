#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.h>
#include <auxiliaries/utils.h>


class Interface {
    public:
        Interface(Level level);
        ~Interface();
        
        void init(SDL_Renderer* renderer);

        void blit(SDL_Renderer* renderer);
        void render(SDL_Renderer* renderer);

        void changeLevel(Level level);

    private:
        void setupLevelMapping();

        void renderBackground(SDL_Renderer* renderer);
        void renderLevel(SDL_Renderer* renderer);

        void loadLevel();

        Level level;

        // Containing all level-loading methods.
        std::unordered_map<Level, std::string> levelMapping;
        // Simulate all tiles presented on the window. Usable as a mapping.
        TileCollection tileCollection;
        SDL_Texture* texture = nullptr;
};