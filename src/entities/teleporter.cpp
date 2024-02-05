#include <entities.hpp>

#include <filesystem>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Teleporter::Teleporter(SDL_Point const& destCoords) : GenericTeleporterEntity<Teleporter>(destCoords) {}


template <>
const std::filesystem::path AbstractEntity<Teleporter>::kTilesetPath = config::entities::teleporter::path;