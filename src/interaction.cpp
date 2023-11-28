#include <interaction.hpp>

#include <interface.hpp>
#include <entities.hpp>


/**
 * @brief Check whether a player entity collides with a telepoter entity.
*/
Teleporter* interaction::collision::checkCollision(const Player& player, IngameInterface& interface) {
    static auto it = Teleporter::instances.end();
    if (!player.isNextTileReached) {
        it = std::find_if(
            Teleporter::instances.begin(), Teleporter::instances.end(),
            [&](const auto& instance) { return instance->destCoords.x == player.nextDestCoords->x && instance->destCoords.y == player.nextDestCoords->y; }
        );
    }

    if (it == Teleporter::instances.end() || !player.isNextTileReached) return nullptr;
    
    Teleporter* target = *it;
    it = Teleporter::instances.end();   // Without this the program magically terminates
    return target;
}

/**
 * @brief Initiated when a player entity collides with a telepoter entity.
*/
void interaction::collision::onCollision(const Teleporter& teleporter, IngameInterface& interface, level::LevelData& currentLevelData) {
    interface.changeLevel(teleporter.targetLevel);
    currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;
}