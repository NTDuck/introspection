#include <interface.hpp>

#include <memory>
#include <tuple>
#include <array>

#include <SDL.h>

#include <components.hpp>
#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Populate members.
*/
MenuInterface::MenuInterface() {
    AnimatedBackground::instantiate(IngameInterface::instance->texture, globals::windowSize);
    Avatar::instantiate(*Teleporter::tilesetData, config::avatar::destRectModifier);
    Button::instantiate(config::button::initializer);
    Title::instantiate(config::title::initializer);
}

void MenuInterface::initialize() {
    Button::initialize();
    Title::initialize();
}

void MenuInterface::deinitialize() {
    AnimatedBackground::deinitialize();
    Avatar::deinitialize();
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
    AnimatedBackground::invoke(&AnimatedBackground::updateAnimation);
    AnimatedBackground::invoke(&AnimatedBackground::render);
    Avatar::invoke(&Avatar::render);
    Button::invoke(&Button::render);
    Title::invoke(&Title::render);
}

void MenuInterface::onWindowChange() {
    AnimatedBackground::invoke(&AnimatedBackground::onWindowChange, globals::windowSize);
    Avatar::invoke(&Avatar::onWindowChange);
    Button::invoke(&Button::onWindowChange);
    Title::invoke(&Title::onWindowChange);
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    Button::invoke(&Button::handleMouseEvent, event);
}