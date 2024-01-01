#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuTitle::MenuTitle(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset) : GenericTitle<MenuTitle>(content, center, preset) {}


template <>
const double GenericTextArea<MenuTitle>::kDestSizeMultiplier = config::title::destSizeMultiplier;

template <>
const std::filesystem::path GenericTextArea<MenuTitle>::fontPath = config::path::fontOmoriHarmonic;