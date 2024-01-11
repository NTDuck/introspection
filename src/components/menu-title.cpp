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
const double GenericComponent<MenuTitle>::kDestSizeModifier = config::components::menu_title::destSizeModifier;

template <>
const SDL_Point GenericComponent<MenuTitle>::kDestRectRatio = config::components::menu_title::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<MenuTitle>::fontPath = config::components::menu_title::fontPath;