#include <interface.hpp>

#include <memory>
#include <tuple>
#include <array>

#include <SDL.h>

#include <components.hpp>
#include <meta.hpp>


/**
 * @brief Populate protected member `buttons`.
 * @note Might move to `globals::config` instead.
*/
MenuInterface::MenuInterface() {
    Button::instantiate(globals::config::kMenuButtonInitializer);
    title = Title::instantiate(globals::config::kTitleInitializer);
}

void MenuInterface::initialize() {
    Button::initialize();
    Title::initialize();
}

void MenuInterface::deinitialize() {
    Button::deinitialize();
    Title::deinitialize();
}

void MenuInterface::render() const {
    renderBackground();
    renderComponents();
}

/**
 * @note In future commits, this method will take advantage of `texture`.
*/
void MenuInterface::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, globals::config::kDefaultBackgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void MenuInterface::renderComponents() const {
    Button::callOnEach(&Button::render);
    title->render();
}

void MenuInterface::onWindowChange() {
    Button::callOnEach(&Button::onWindowChange);
    title->onWindowChange();
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    Button::callOnEach(&Button::handleMouseEvent, event);
}