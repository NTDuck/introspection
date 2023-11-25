#include <interaction.hpp>

#include <interface.hpp>
#include <entities.hpp>


namespace interaction {
    namespace collision {
        void PlayerCollideTeleporter(IngameInterface& interface, const Teleporter& teleporter, level::LevelData& currentLevelData) {
            interface.changeLevel(teleporter.targetLevel);
            currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;
        }
    }
}