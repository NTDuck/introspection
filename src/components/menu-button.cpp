#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuButton::MenuButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState) : GenericComponent<MenuButton>(center, onMouseOutPreset), GenericButtonComponent<MenuButton>(center, onMouseOutPreset, onMouseOverPreset, content, destState) {}


template <>
const double GenericComponent<MenuButton>::kDestSizeModifier = 1;

template <>
const std::filesystem::path GenericTextComponent<MenuButton>::fontPath = config::path::fontOmoriHarmonic;