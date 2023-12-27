#include <interface.hpp>


GenericButton::GenericButton(GameState* destState, std::string const& content, SDL_FPoint const& center) : GenericTextArea<GenericButton>(content, center), destState(destState) {}

void GenericButton::handleMouseEvent(SDL_Event const& event) {
    if (!SDL_PointInRect(&globals::mouseState, &outerDestRect)) {
        if (isMouseOut) return;
        isMouseOut = true;
        onMouseOut();
    } else {
        if (isMouseOut) isMouseOut = false;
        if (event.type == SDL_MOUSEBUTTONDOWN) onClick(); else onMouseOver();
    }
}

void GenericButton::onMouseOut() {
    preset = globals::config::kButtonOnMouseOutPreset;
    loadOuterTexture();
    loadInnerTexture();
}

void GenericButton::onMouseOver() {
    preset = globals::config::kButtonOnMouseOverPreset;
    loadOuterTexture();
    loadInnerTexture();
}

void GenericButton::onClick() {
    if (destState == nullptr) return;
    globals::state = *destState;
}