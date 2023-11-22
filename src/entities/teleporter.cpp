#include <SDL.h>

#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries/globals.hpp>


extern template class AnimatedDynamicTextureWrapper<Player>;
extern template class AnimatedTextureWrapper<Teleporter>;


void Teleporter::initialize() {
    BaseTextureWrapper<Teleporter>::initialize(globals::config::TELEPORTER_TILESET_PATH);
}


void Teleporter::onLevelChange(const leveldata::TextureData& teleporterData) {
    auto data = dynamic_cast<const leveldata::TeleporterData*>(&teleporterData);
    
    dynamic_cast<BaseTextureWrapper<Player>*>(this)->onLevelChange(*data);
    targetDestCoords = data->targetDestCoords;
    targetLevel = data->targetLevel;
}

/**
 * @brief Call method `onLevelChange()` on each element in `teleporters`.
*/
void Teleporter::onLevelChangeAll(const leveldata::TeleporterData::TeleporterDataCollection& teleporterDataCollection) {
    // for (const auto& data : teleportersData) {instanceMapping.emplace(data.destCoords, Teleporter{}).first -> second.onLevelChange(data)};
    instanceMapping.clear();

    for (const auto& data : teleporterDataCollection) {
        auto instance = Teleporter::instantiate(data.destCoords);
        instance->onLevelChange(data);
    }
}