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

        void blit(SDL_Renderer* renderer, const SDL_Point WINDOW_SIZE);
        void render(SDL_Renderer* renderer);

        void changeLevel(Level level);

    private:
        void loadSpriteSheets(SDL_Renderer* renderer);
        void setupTileMapping();
        void setupLevelMapping();
        void setupTiles(const SDL_Point WINDOW_SIZE);

        void renderBackground(SDL_Renderer* renderer, const SDL_Point WINDOW_SIZE);
        void renderTiles(SDL_Renderer* renderer);

        void loadLevel();
        void loadLevelChapelOfAnticipation();

        Level level;

        // Containing all level-loading methods.
        std::unordered_map<Level, std::function<void()>> levelMapping;
        // Simulate all tiles presented on the window. Usable as a mapping.
        TileMap tileMap;
        // Convert a `TileType` into its respective coordinates (not indices) on the spritesheet. Directly applicable to `tile.srcRect.x` and `tile.srcRect.y`.
        TileMapping tileMapping;

        SDL_Texture* texture = nullptr;
        std::unordered_map<TileSet, SDL_Texture*> spriteSheets;
};