#include <functional>

#include <SDL.h>

#include <entities.hpp>
#include <auxiliaries/globals.hpp>


Teleporter::Teleporter(SDL_Point destCoords, std::string destLevel) : destCoords(destCoords), destLevel(destLevel) {}

Teleporter::~Teleporter() {}

void Teleporter::teleport(std::function<void(std::string)> loadLevelFunc) {
    loadLevelFunc(destLevel);
    globals::currentLevel.playerDestCoords = destCoords;
}