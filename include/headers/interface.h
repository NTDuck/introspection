#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <auxiliaries/utils.h>


class Interface {
    public:
        Interface(Level level);
        ~Interface();
        
        void init(SDL_Renderer* renderer);
        void render(SDL_Renderer* renderer);

        void loadSpriteSheets(SDL_Renderer* renderer);
        void setupTileMapping();
        void setupLevelMapping();
        void setupTiles();

        void loadLevel();
        void loadLevelChapelOfAnticipation();
        
        Level level;

    private:
        // Containing all level-loading methods.
        std::unordered_map<Level, std::function<void()>> levelMapping;
        // Simulate all tiles presented on the window. Usable as a mapping.
        TileMap tileMap;
        // Convert a `TileType` into its respective coordinates (not indices) on the spritesheet. Directly applicable to `tile.srcRect.x` and `tile.srcRect.y`.
        TileMapping tileMapping;

        SDL_Renderer* renderer = nullptr;
        std::unordered_map<TileSet, SDL_Texture*> spriteSheets;
};