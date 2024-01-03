#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuTitle::MenuTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<MenuTitle>(center, preset), GenericTextComponent<MenuTitle>(center, preset, content) {}


template <>
const double GenericComponent<MenuTitle>::kDestSizeModifier = config::title::destSizeModifier;

template <>
const std::filesystem::path GenericTextComponent<MenuTitle>::fontPath = config::path::fontOmoriHarmonic;