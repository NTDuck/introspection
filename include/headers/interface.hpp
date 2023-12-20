#ifndef INTERFACE_H
#define INTERFACE_H

#include <unordered_map>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Represent the in-game interface.
*/
class IngameInterface final : public AbstractInterface<IngameInterface> {
    friend AbstractInterface<IngameInterface>;
    public:
        using AbstractInterface<IngameInterface>::render, AbstractInterface<IngameInterface>::texture;

        IngameInterface(const level::LevelName levelName);
        ~IngameInterface() = default;

        static void initialize();
        
        void changeLevel(const level::LevelName levelName);
        void onLevelChange();
        void onWindowChange();

    private:
        void loadLevel();
        void renderBackground();
        void renderLevelTiles();

        level::LevelName levelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        static level::LevelMapping levelMapping;
};


#endif