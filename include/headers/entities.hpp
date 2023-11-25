#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief A singleton class representing one instance of the player entity.
 * @note Only one instance should exist at a time.
*/
class Player final : public AbstractAnimatedDynamicEntity<Player> {
    friend AbstractAnimatedDynamicEntity<Player>;
    public:
        static Player* instantiate();
        Player();
        ~Player() = default;

        static void initialize();
        static void deinitialize();
        
        void onLevelChange(const level::EntityLevelData& player) override;
        void handleKeyboardEvent(const SDL_Event& event);

    private:
        static Player* instance;
};


/**
 * @brief A multiton class representing controlled instances of teleporter entities.
*/
class Teleporter final : public AbstractAnimatedEntity<Teleporter> {
    friend AbstractAnimatedEntity<Teleporter>;
    public:
        Teleporter();
        ~Teleporter() = default;
        static void initialize();

        void onLevelChange(const level::EntityLevelData& teleporterData) override;
        static void onLevelChange(const level::TeleporterLevelData::Collection& teleporterDataCollection);

        /**
         * The new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
        */
        SDL_Point targetDestCoords;

        /**
         * The new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
        */
        level::LevelName targetLevel;
};


#endif