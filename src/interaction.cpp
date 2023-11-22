#include <interaction.hpp>
#include <interface.hpp>
#include <entities.hpp>


extern template class AnimatedDynamicTextureWrapper<Player>;
extern template class AnimatedTextureWrapper<Teleporter>;

/**
 * @brief Handle interaction between entities.
*/
namespace interaction {
    /**
     * @brief Handle collision between entities.
    */
    namespace collision {
        void PlayerCollideTeleporter(Interface& interface, const Teleporter& teleporter, leveldata::LevelData& currentLevelData) {
            interface.changeLevel(teleporter.targetLevel);
            currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;
        }
    }
}