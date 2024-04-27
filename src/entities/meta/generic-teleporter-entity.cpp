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
    handleCustomEventPOST_impl<event::Code::kReq_Teleport_GTE_Player>();
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_Teleport_GTE_Player>
GenericTeleporterEntity<T>::handleCustomEventPOST_impl() const {
    auto event = event::instantiate();
    event::setID(event, mID);
    event::setCode(event, C);
    event::setData(event, event::Data_Teleporter({ mDestCoords, mTargetDestCoords, mTargetLevel }));
    event::enqueue(event);
}


template class GenericTeleporterEntity<PlaceholderTeleporter>;
template class GenericTeleporterEntity<RedHandThrone>;


DEF_GENERIC_TELEPORTER_ENTITY(PlaceholderTeleporter, config::entities::placeholders::teleporter)
DEF_GENERIC_TELEPORTER_ENTITY(RedHandThrone, config::entities::teleporter::red_hand_throne)