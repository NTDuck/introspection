#include <interface.hpp>

#include <functional>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameViewHandler::IngameViewHandler(std::function<void()> const& callable, SDL_Rect& destRect, const IngameViewMode viewMode) : AbstractInterface<IngameViewHandler>(), callable(callable), viewMode(viewMode), destRect(destRect) {}

void IngameViewHandler::render() const {
    switch (viewMode) {
        case IngameViewMode::kFullScreen:
            std::invoke(callable);
            break;

        case IngameViewMode::kFocusOnEntity:
            // Focus on player entity
            SDL_SetRenderTarget(globals::renderer, texture);

            // Render dependencies
            std::invoke(callable);

            // Calculate rendered portion
            srcRect.x = destRect.x + destRect.w / 2 - srcRect.w / 2;
            srcRect.y = destRect.y + destRect.h / 2 - srcRect.h / 2;

            // "Fix" out-of-bound cases
            if (srcRect.x < 0) srcRect.x = 0;
            else if (srcRect.x + srcRect.w > globals::windowSize.x) srcRect.x = globals::windowSize.x - srcRect.w;
            if (srcRect.y < 0) srcRect.y = 0;
            else if (srcRect.y + srcRect.h > globals::windowSize.y) srcRect.y = globals::windowSize.y - srcRect.h;

            SDL_SetRenderTarget(globals::renderer, nullptr);
            SDL_RenderCopy(globals::renderer, texture, &srcRect, nullptr);
            break;
    }
}

void IngameViewHandler::onWindowChange() {
    AbstractInterface<IngameViewHandler>::onWindowChange();
    
    tileCountWidth = static_cast<double>(globals::windowSize.x) / static_cast<double>(globals::windowSize.y) * tileCountHeight;
    srcRect.w = tileCountWidth * globals::tileDestSize.x;
    srcRect.h = tileCountHeight * globals::tileDestSize.y;
}

/**
 * @see https://stackoverflow.com/questions/12761315/random-element-from-unordered-set-in-o1
*/
void IngameViewHandler::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case config::key::INGAME_TOGGLE_CAMERA_ANGLE:
            if (event.type != SDL_KEYDOWN) break;
            switchViewMode(viewMode == IngameViewMode::kFocusOnEntity ? IngameViewMode::kFullScreen : IngameViewMode::kFocusOnEntity);
            break;

        default: break;
    }
}

void IngameViewHandler::switchViewMode(const IngameViewMode newViewMode) {
    if (viewMode == newViewMode) return;
    viewMode = newViewMode;
}