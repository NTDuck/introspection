#include <SDL.h>

#include <entities.hpp>
#include <auxiliaries/globals.hpp>


Teleporter::Teleporter() {}

Teleporter::~Teleporter() { BaseTextureWrapper::~BaseTextureWrapper(); }

void Teleporter::onLevelChange(const globals::leveldata::TextureData& teleporter) {
    auto data = dynamic_cast<const globals::leveldata::TeleporterData*>(&teleporter);
    
    BaseTextureWrapper::onLevelChange(*data);
    targetDestCoords = data -> targetDestCoords;
    targetLevel = data -> targetLevel;
}

/**
 * @brief Call method `onLevelChange()` on each element in `teleporters`.
*/
void Teleporter::onLevelChange_(Teleporters& teleporters, const globals::leveldata::TeleportersData& teleportersData) {
    for (const auto& data : teleportersData) teleporters.emplace(data.destCoords, Teleporter{}).first -> second.onLevelChange(data);
}

/**
 * @brief Call method `onWindowChange()` on each element in `teleporters`.
*/
void Teleporter::onWindowChange_(Teleporters& teleporters) {
    for (auto& pair : teleporters) pair.second.onWindowChange();
}