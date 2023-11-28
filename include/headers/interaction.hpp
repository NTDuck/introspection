#ifndef INTERACTION_H
#define INTERACTION_H

#include <interface.hpp>
#include <entities.hpp>


/**
 * @brief Handle interaction between entities.
*/
namespace interaction {
    /**
     * @brief Handle collision between entities.
    */
    namespace collision {
        Teleporter* checkCollision(const Player& player, IngameInterface& interface);
        void onCollision(const Teleporter& teleporter, IngameInterface& interface, level::LevelData& currentLevelData);
    }
}


#endif