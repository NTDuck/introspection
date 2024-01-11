#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


FrameRateOverlay::FrameRateOverlay(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<FrameRateOverlay>(center, preset), GenericTextBoxComponent<FrameRateOverlay>(center, preset, content) {}

void FrameRateOverlay::deinitialize() {
    Singleton<FrameRateOverlay>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}


template <>
const double GenericComponent<FrameRateOverlay>::kDestSizeModifier = config::text::destSizeModifierFrameRateOverlay;

template <>
const SDL_Point GenericComponent<FrameRateOverlay>::kDestRectRatio = config::button::frameRateOverlayDestRectRatio;

template <>
const std::filesystem::path GenericTextComponent<FrameRateOverlay>::fontPath = config::path::fontOmoriHarmonic;