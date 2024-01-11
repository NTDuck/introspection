#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


FPSOverlay::FPSOverlay(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<FPSOverlay>(center, preset), GenericTextBoxComponent<FPSOverlay>(center, preset, content) {}

void FPSOverlay::deinitialize() {
    Singleton<FPSOverlay>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}


template <>
const double GenericComponent<FPSOverlay>::kDestSizeModifier = config::components::fps_overlay::destSizeModifier;

template <>
const SDL_Point GenericComponent<FPSOverlay>::kDestRectRatio = config::components::fps_overlay::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<FPSOverlay>::fontPath = config::components::fps_overlay::fontPath;