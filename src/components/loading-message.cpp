#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


LoadingMessage::LoadingMessage(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<LoadingMessage>(center, preset), GenericTextComponent<LoadingMessage>(center, preset, content) {}

void LoadingMessage::deinitialize() {
    Singleton<LoadingMessage>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}


template <>
const double GenericComponent<LoadingMessage>::kDestSizeModifier = config::text::destSizeModifierLoadingMessage;

template <>
const SDL_Point GenericComponent<LoadingMessage>::kDestRectRatio = config::button::defaultDestRectRatio;

template <>
const std::filesystem::path GenericTextComponent<LoadingMessage>::fontPath = config::path::fontOmoriHarmonic;