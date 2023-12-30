#include <components.hpp>

#include <filesystem>
#include <string>

#include <SDL.h>

#include <auxiliaries.hpp>


Title::Title(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset) : TextArea<Title>(content, center, preset) {}

void Title::deinitialize() {
    Singleton<Title>::deinitialize();
    TTF_CloseFont(font);
    font = nullptr;
}

void Title::render() const {
    SDL_RenderCopy(globals::renderer, innerTexture, nullptr, &innerDestRect);
}


template <>
const double TextArea<Title>::kDestSizeMultiplier = 5.5;

template <>
const std::filesystem::path TextArea<Title>::fontPath = config::path::fontOmoriHarmonic;