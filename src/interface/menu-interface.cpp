#include <interface.hpp>

#include <memory>
#include <tuple>
#include <array>

#include <SDL.h>

#include <components.hpp>
#include <meta.hpp>


/**
 * @brief Populate members.
*/
MenuInterface::MenuInterface() {
    Button::instantiate(config::button::initializer);
    Title::instantiate(config::title::initializer);
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
    utils::setRendererDrawColor(globals::renderer, config::color::offblack);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void MenuInterface::renderComponents() const {
    Button::invoke(&Button::render);
    Title::invoke(&Title::render);
}

void MenuInterface::onWindowChange() {
    Button::invoke(&Button::onWindowChange);
    Title::invoke(&Title::onWindowChange);
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    Button::invoke(&Button::handleMouseEvent, event);
}