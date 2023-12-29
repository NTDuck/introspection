#include <components.hpp>

#include <string>
#include <SDL.h>

#include <auxiliaries.hpp>


Button::Button(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver) : TextArea<Button>(content, center, preset), kDestState(destState), kPresetOnMouseOver(presetOnMouseOver) {}

void Button::render() const {
    SDL_RenderCopy(globals::renderer, (isMouseOut ? outerTexture : outerTextureOnMouseOver), nullptr, &outerDestRect);
    SDL_RenderCopy(globals::renderer, (isMouseOut ? innerTexture : innerTextureOnMouseOver), nullptr, &innerDestRect);
}

void Button::onWindowChange() {
    TextArea<Button>::onWindowChange();
    loadOuterTexture(outerTextureOnMouseOver, kPresetOnMouseOver);
    loadInnerTexture(innerTextureOnMouseOver, globals::config::kButtonOnMouseOverPreset);
}

void Button::handleMouseEvent(SDL_Event const& event) {
    isMouseOut = !SDL_PointInRect(&globals::mouseState, &outerDestRect);
    if (!isMouseOut && event.type == SDL_MOUSEBUTTONDOWN) onClick();
}

void Button::onClick() {
    if (kDestState == nullptr) return;
    globals::state = *kDestState;
}


template <>
const double TextArea<Button>::kDestSizeMultiplier = 1;

template <>
const std::filesystem::path TextArea<Button>::fontPath = globals::config::kOmoriFontSecondPath;