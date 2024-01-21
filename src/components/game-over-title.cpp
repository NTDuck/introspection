#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


GameOverTitle::GameOverTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<GameOverTitle>(center, preset), GenericTextComponent<GameOverTitle>(center, preset, content) {}

void GameOverTitle::deinitialize() {
    Singleton<MenuTitle>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}


template <>
const double GenericComponent<GameOverTitle>::kDestSizeModifier = config::components::game_over_title::destSizeModifier;

template <>
const SDL_Point GenericComponent<GameOverTitle>::kDestRectRatio = config::components::game_over_title::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<GameOverTitle>::fontPath = config::components::game_over_title::fontPath;