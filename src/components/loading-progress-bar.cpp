#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


LoadingProgressBar::LoadingProgressBar(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<LoadingProgressBar>(center, preset), GenericProgressBarComponent<LoadingProgressBar>(center, preset) {}

void LoadingProgressBar::deinitialize() {
    Singleton<LoadingProgressBar>::deinitialize();
}


template <>
const double GenericComponent<LoadingProgressBar>::kDestSizeModifier = 1;