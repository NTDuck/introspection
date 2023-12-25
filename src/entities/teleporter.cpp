#include <entities.hpp>

#include <filesystem>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Teleporter::Teleporter() {
    destRectModifier = globals::config::kDefaultTeleporterDestRectModifier;
}

void Teleporter::onLevelChange(const level::EntityLevelData& teleporterData) {
    auto data = dynamic_cast<const level::TeleporterLevelData*>(&teleporterData);
    AbstractAnimatedEntity<Teleporter>::onLevelChange(*data);

    targetDestCoords = data->targetDestCoords;
    targetLevel = data->targetLevel;
}


template <>
const std::filesystem::path AbstractEntity<Teleporter>::kTilesetPath = globals::config::kTilesetPathTeleporter;