#include <entities.hpp>

#include <SDL.h>

#include <components.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericInteractable<T>::GenericInteractable(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {}

template <typename T>
void GenericInteractable<T>::onLevelChange(level::Data_Generic const& interactableData) {
    auto data = *reinterpret_cast<const level::Data_Interactable*>(&interactableData);

    mDialogues = data.dialogues;
}

template <typename T>
void GenericInteractable<T>::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
        case event::Code::kReq_Interact_Player_GIE:
            handleCustomEventGET_kReq_Interact_Player_GIE(event);
            break;
        
        default: break;
    }
}

template <typename T>
void GenericInteractable<T>::handleCustomEventGET_kReq_Interact_Player_GIE(SDL_Event const& event) {
    auto data = event::getData<event::Data_Interactable>(event);
    if (data.targetDestCoords != mDestCoords) return;

    IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, mDialogues);
}


template class GenericInteractable<Interactable>;
template class GenericInteractable<OmoriLaptop>;
template class GenericInteractable<OmoriMewO>;


DEFINE_GENERIC_INTERACTABLE(Interactable, config::entities::interactable)
DEFINE_GENERIC_INTERACTABLE(OmoriLaptop, config::entities::omori_laptop)
DEFINE_GENERIC_INTERACTABLE(OmoriMewO, config::entities::omori_mewo)