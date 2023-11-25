#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries/globals.hpp>


Teleporter::Teleporter() {
    destRectModifier.x -= globals::tileDestSize.x;
}

void Teleporter::initialize() {
    AbstractAnimatedEntity<Teleporter>::initialize(globals::config::kTilesetPathTeleporter);
}

void Teleporter::onLevelChange(const level::EntityLevelData& teleporterData) {
    auto data = dynamic_cast<const level::TeleporterLevelData*>(&teleporterData);
    
    AbstractAnimatedEntity<Teleporter>::onLevelChange(*data);

    targetDestCoords = data->targetDestCoords;
    targetLevel = data->targetLevel;
}

/**
 * @brief Call method `onLevelChange()` on each element in `teleporters`.
*/
void Teleporter::onLevelChange(const level::TeleporterLevelData::Collection& teleporterDataCollection) {
    instanceMapping.clear();

    for (const auto& data : teleporterDataCollection) {
        auto instance = instantiate(data.destCoords);
        instance->onLevelChange(data);
    }
}