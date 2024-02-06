#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericTeleporterEntity<T>::GenericTeleporterEntity(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {
    destRectModifier = config::entities::teleporter::destRectModifier;
    primaryStats = config::entities::teleporter::primaryStats;
}

template <typename T>
void GenericTeleporterEntity<T>::onLevelChange(level::EntityLevelData const& teleporterData) {
    auto data = *reinterpret_cast<const level::TeleporterLevelData*>(&teleporterData);
    AbstractAnimatedEntity<T>::onLevelChange(data);

    targetDestCoords = data.targetDestCoords;
    targetLevel = data.targetLevel;
}

template <typename T>
void GenericTeleporterEntity<T>::handleCustomEventPOST() const {
    handleCustomEventPOST_kReq_Teleport_GTE_Player();
}

template <typename T>
void GenericTeleporterEntity<T>::handleCustomEventPOST_kReq_Teleport_GTE_Player() const {
    auto event = event::instantiate();
    event::setID(event, id);
    event::setCode(event, event::Code::kReq_Teleport_GTE_Player);
    event::setData(event, event::data::Teleporter({ destCoords, targetDestCoords, targetLevel }));
    event::enqueue(event);
}


template class GenericTeleporterEntity<Teleporter>;