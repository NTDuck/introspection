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
class Player final : public Singleton<Player>, public AbstractAnimatedDynamicEntity<Player> {
    friend AbstractAnimatedDynamicEntity<Player>;
    public:
        using Singleton<Player>::instantiate, Singleton<Player>::instance;

        Player();
        ~Player() = default;

        static void deinitialize();
        
        void onLevelChange(const level::EntityLevelData& player) override;
        void handleKeyboardEvent(const SDL_Event& event);
};


/**
 * @brief A multiton class representing controlled instances of teleporter entities.
*/
class Teleporter final : public AbstractAnimatedEntity<Teleporter> {
    friend AbstractAnimatedDynamicEntity<Teleporter>;
    public:
        using AbstractEntity<Teleporter>::instantiate;

        Teleporter();
        ~Teleporter() = default;

        void onLevelChange(const level::EntityLevelData& teleporterData) override;

        /**
         * The new `destCoords` of the player entity upon a `destCoords` collision event i.e. "trample".
        */
        SDL_Point targetDestCoords;

        /**
         * The new `level::LevelName` to be switched to upon a `destCoords` collision event i.e. "trample".
        */
        level::LevelName targetLevel;
};


/**
 * @brief A multiton class representing controlled instances of slime entities.
*/
class Slime final : public AbstractAnimatedDynamicEntity<Slime> {
    friend AbstractAnimatedDynamicEntity<Slime>;
    public:
        using AbstractEntity<Slime>::instantiate;

        Slime();
        ~Slime() = default;

        void calculateMove(const SDL_Point& playerDestCoords);

    private:
        /**
         * If the player entity is "within" the specified range, the slime entity would move towards the player. The slime would remain IDLE otherwise.
        */
        SDL_Point kMoveInitiateRange;
};


#endif