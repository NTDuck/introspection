#include <components.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


LoadingProgressBar::LoadingProgressBar(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<LoadingProgressBar>(center, preset), GenericProgressBarComponent<LoadingProgressBar>(center, preset) {}

void LoadingProgressBar::deinitialize() {
    Singleton<LoadingProgressBar>::deinitialize();
}


template <>
const double GenericComponent<LoadingProgressBar>::kDestSizeModifier = config::components::loading_progress_bar::destSizeModifier;

template <>
const SDL_Point GenericComponent<LoadingProgressBar>::kDestRectRatio = config::components::loading_progress_bar::destRectRatio;

template <>
const double GenericProgressBarComponent<LoadingProgressBar>::kProgressUpdateRateLimit = config::components::loading_progress_bar::progressUpdateRateLimit;

template <>
const double GenericProgressBarComponent<LoadingProgressBar>::kProgressUpdateRate = config::components::loading_progress_bar::progressUpdateRate;