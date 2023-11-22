#pragma once

#include <string>
#include <unordered_map>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Represents the player.
 * @note Only one instance should exist at a time.
*/
class Player : public AnimatedDynamicTextureWrapper<Player> {
    public:
        static void instantiate();
        ~Player() = default;
        static void initialize();
        
        void onLevelChange(const leveldata::TextureData& player) override;
        void handleKeyboardEvent(const SDL_Event& event);

        static Player* instance;

    private:
        Player();
};


class Teleporter : public AnimatedTextureWrapper<Teleporter> {
    public:
        static void initialize();

        void onLevelChange(const leveldata::TextureData& teleporterData) override;
        static void onLevelChangeAll(const leveldata::TeleporterData::TeleporterDataCollection& teleporterDataCollection);

        SDL_Point targetDestCoords;
        std::string targetLevel;

    private:
        Teleporter() = default;
        ~Teleporter() = default;
};