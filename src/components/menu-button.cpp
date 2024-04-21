#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuButton::MenuButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState, std::function<void(void)> const& callback) : GenericComponent<MenuButton>(center, onMouseOutPreset), GenericButtonComponent<MenuButton>(center, onMouseOutPreset, onMouseOverPreset, content, destState, callback) {}


template <>
const double GenericComponent<MenuButton>::kDestSizeModifier = config::components::menu_button::destSizeModifier;

template <>
const SDL_Point GenericComponent<MenuButton>::kDestRectRatio = config::components::menu_button::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<MenuButton>::sFontPath = config::components::menu_button::fontPath;