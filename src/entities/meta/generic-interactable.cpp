#include <entities.hpp>

#include <SDL.h>

#include <components.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename T>
GenericInteractable<T>::GenericInteractable(SDL_Point const& destCoords) : AbstractAnimatedEntity<T>(destCoords) {}

template <typename T>
void GenericInteractable<T>::deinitialize() {
    if (sSFXName != nullptr) {
        delete sSFXName;
        sSFXName = nullptr;
    }

    AbstractAnimatedEntity<T>::deinitialize();
}

template <typename T>
void GenericInteractable<T>::onLevelChange(level::Data_Generic const& interactableData) {
    auto data = *reinterpret_cast<const level::Data_Interactable*>(&interactableData);

    mDialogues = data.dialogues;
}

template <typename T>
void GenericInteractable<T>::handleCustomEventGET(SDL_Event const& event) {
    switch (event::getCode(event)) {
        case event::Code::kReq_Interact_Player_GIE:
            handleCustomEventGET_impl<event::Code::kReq_Interact_Player_GIE>(event);
            break;
        
        default: break;
    }
}

template <typename T>
template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_Interact_Player_GIE>
GenericInteractable<T>::handleCustomEventGET_impl(SDL_Event const& event) {
    auto data = event::getData<event::Data_Interactable>(event);
    if (data.targetDestCoords != mDestCoords) return;

    // Halving `mProgress` as a temporary patch for redundant calls (which leads to dialogues being unwantedly skipped)
    IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, mDialogues[mProgress >> 1]);
    if (mProgress >> 1 < static_cast<unsigned short int>(mDialogues.size()) - 1) ++mProgress;   // Move towards final state

    if (sSFXName != nullptr && (mProgress & 1)) Mixer::invoke(&Mixer::playSFX, *sSFXName);     
}


template class GenericInteractable<PlaceholderInteractable>;
template class GenericInteractable<OmoriLaptop>;
template class GenericInteractable<OmoriMewO>;
template class GenericInteractable<OmoriCat_0>;
template class GenericInteractable<OmoriCat_1>;
template class GenericInteractable<OmoriCat_2>;
template class GenericInteractable<OmoriCat_3>;
template class GenericInteractable<OmoriCat_4>;
template class GenericInteractable<OmoriCat_5>;
template class GenericInteractable<OmoriCat_6>;
template class GenericInteractable<OmoriCat_7>;


DEF_GENERIC_INTERACTABLE(PlaceholderInteractable, config::entities::placeholders::interactable)
DEF_GENERIC_INTERACTABLE(OmoriLaptop, config::entities::interactables::omori_laptop)
DEF_GENERIC_INTERACTABLE_SFX(OmoriMewO, config::entities::interactables::omori_mewo, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_0, config::entities::interactables::omori_cat_0, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_1, config::entities::interactables::omori_cat_1, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_2, config::entities::interactables::omori_cat_2, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_3, config::entities::interactables::omori_cat_3, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_4, config::entities::interactables::omori_cat_4, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_5, config::entities::interactables::omori_cat_5, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_6, config::entities::interactables::omori_cat_6, new Mixer::SFXName(Mixer::SFXName::kMewo))
DEF_GENERIC_INTERACTABLE_SFX(OmoriCat_7, config::entities::interactables::omori_cat_7, new Mixer::SFXName(Mixer::SFXName::kMewo))