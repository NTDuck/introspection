#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Represents the player.
*/
class Player : public NonStaticTextureWrapper {
    public:
        Player();
        ~Player();

        void init();
        void handleKeyboardEvent(const SDL_Event& event);

        void render() override;
};


class Teleporter {
    public:
        Teleporter(SDL_Point destCoords, std::string destLevel);
        ~Teleporter();

        void teleport(std::function<void(std::string)> loadLevelFunc);

        SDL_Point destCoords;
        std::string destLevel;
};