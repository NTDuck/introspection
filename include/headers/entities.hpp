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
class Player : public AnimatedDynamicTextureWrapper {
    public:
        Player();
        ~Player();

        void init();
        void render() override;
        
        void onLevelChange(const globals::levelData::Texture& player) override;
        void handleKeyboardEvent(const SDL_Event& event);
};


class Teleporter : public BaseTextureWrapper {
    public:
        Teleporter();
        ~Teleporter();

        void onLevelChange(const globals::levelData::Texture& teleporter) override;
        void teleport(std::function<void(std::string)>& loadLevelFunc);

        SDL_Point targetDestCoords;
        std::string targetLevel;
};