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
        void PlayerCollideTeleporter(IngameInterface& interface, const Teleporter& teleporter, level::LevelData& currentLevelData);
    }
}


#endif