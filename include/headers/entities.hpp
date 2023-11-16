#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
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
        
        void onLevelChange(const leveldata::TextureData& player) override;
        void handleKeyboardEvent(const SDL_Event& event);
};


class Teleporter : public BaseTextureWrapper {
    public:
        Teleporter();
        ~Teleporter();

        using Teleporters = std::unordered_map<SDL_Point, Teleporter, utils::hashers::SDL_Point_Hasher, utils::operators::SDL_Point_Equality_Operator>;

        void onLevelChange(const leveldata::TextureData& teleporter) override;
        static void onLevelChange_(Teleporters& teleporters, const leveldata::TeleporterData::TeleporterDataCollection& teleportersData);
        static void onWindowChange_(Teleporters& teleporters);

        SDL_Point targetDestCoords;
        std::string targetLevel;
};