#include <functional>

#include <SDL.h>

#include <entities.hpp>
#include <auxiliaries/globals.hpp>


Teleporter::Teleporter() {}

Teleporter::~Teleporter() {
    BaseTextureWrapper::~BaseTextureWrapper();
}

/**
 * @brief Switch to new level specified by the instance's attributes.
 * @bug Apparently does not work on classmethods.
*/
void Teleporter::teleport(std::function<void(std::string)>& loadLevelFunc) {
    loadLevelFunc(targetLevel);
    globals::currentLevel.player.destCoords = targetDestCoords;
}

void Teleporter::onLevelChange(const globals::levelData::Texture& teleporter) {
    auto data = dynamic_cast<const globals::levelData::Teleporter*>(&teleporter);
    
    BaseTextureWrapper::onLevelChange(*data);
    targetDestCoords = data -> targetDestCoords;
    targetLevel = data -> targetLevel;
}