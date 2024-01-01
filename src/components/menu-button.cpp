#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


MenuButton::MenuButton(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver) : GenericButton<MenuButton>(destState, content, center, preset, presetOnMouseOver) {}


template <>
const double GenericTextArea<MenuButton>::kDestSizeMultiplier = 1;

template <>
const std::filesystem::path GenericTextArea<MenuButton>::fontPath = config::path::fontOmoriHarmonic;