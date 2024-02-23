#include <entities.hpp>

#include <SDL.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericTeleporterEntity<T>::GenericTeleporterEntity(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {}

template <typename T>
void GenericTeleporterEntity<T>::onLevelChange(level::Data_Generic const& teleporterData) {
    auto data = *reinterpret_cast<const level::Data_Teleporter*>(&teleporterData);
    AbstractAnimatedEntity<T>::onLevelChange(data);

    mTargetDestCoords = data.targetDestCoords;
    mTargetLevel = data.targetLevel;
}

template <typename T>
void GenericTeleporterEntity<T>::handleCustomEventPOST() const {
    handleCustomEventPOST_kReq_Teleport_GTE_Player();
}

template <typename T>
void GenericTeleporterEntity<T>::handleCustomEventPOST_kReq_Teleport_GTE_Player() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, event::Code::kReq_Teleport_GTE_Player);
    event::setData(event, event::Data_Teleporter({ mDestCoords, mTargetDestCoords, mTargetLevel }));
    event::enqueue(event);
}


template class GenericTeleporterEntity<Teleporter>;
template class GenericTeleporterEntity<RedHandThrone>;


DEFINE_GENERIC_TELEPORTER_ENTITY(Teleporter, config::entities::teleporter)
DEFINE_GENERIC_TELEPORTER_ENTITY(RedHandThrone, config::entities::teleporter_red_hand_throne)