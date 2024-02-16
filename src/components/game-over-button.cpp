#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


GameOverButton::GameOverButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState) : GenericComponent<GameOverButton>(center, onMouseOutPreset), GenericButtonComponent<GameOverButton>(center, onMouseOutPreset, onMouseOverPreset, content, destState) {}

void GameOverButton::deinitialize() {
    GenericButtonComponent<GameOverButton>::deinitialize();
    Singleton<GameOverButton>::deinitialize();
}


template <>
const double GenericComponent<GameOverButton>::kDestSizeModifier = config::components::game_over_button::destSizeModifier;

template <>
const SDL_Point GenericComponent<GameOverButton>::kDestRectRatio = config::components::game_over_button::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<GameOverButton>::sFontPath = config::components::game_over_button::fontPath;