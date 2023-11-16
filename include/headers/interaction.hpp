#pragma once

#include <interface.hpp>
#include <entities.hpp>


namespace interaction {
    namespace collision {
        void PlayerCollideTeleporter(Interface& interface, const Teleporter& teleporter, leveldata::LevelData& currentLevelData);
    }
}