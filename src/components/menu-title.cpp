#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuTitle::MenuTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<MenuTitle>(center, preset), GenericTextComponent<MenuTitle>(center, preset, content) {}

void MenuTitle::deinitialize() {
    Singleton<MenuTitle>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}


template <>
const double GenericComponent<MenuTitle>::kDestSizeModifier = config::text::destSizeModifierMenuTitle;

template <>
const SDL_Point GenericComponent<MenuTitle>::kDestRectRatio = config::button::defaultDestRectRatio;

template <>
const std::filesystem::path GenericTextComponent<MenuTitle>::fontPath = config::path::fontOmoriHarmonic;