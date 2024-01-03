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
    MenuAnimatedBackground::instantiate(IngameInterface::instance->texture);
    MenuAvatar::instantiate(*Teleporter::tilesetData, config::avatar::destRectModifier);
    MenuButton::instantiate(config::button::initializerMenuButton);
    MenuTitle::instantiate(config::title::initializerMenuTitle);
}

void MenuInterface::initialize() {
    // MenuButton::initialize();
    // MenuTitle::initialize();
}

void MenuInterface::deinitialize() {
    MenuAnimatedBackground::deinitialize();
    MenuAvatar::deinitialize();
    MenuButton::deinitialize();
    MenuTitle::deinitialize();
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
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::updateAnimation);
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::render);
    MenuAvatar::invoke(&MenuAvatar::render);
    MenuButton::invoke(&MenuButton::render);
    MenuTitle::invoke(&MenuTitle::render);
}

void MenuInterface::onWindowChange() {
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::onWindowChange);
    MenuAvatar::invoke(&MenuAvatar::onWindowChange);
    MenuButton::invoke(&MenuButton::onWindowChange);
    MenuTitle::invoke(&MenuTitle::onWindowChange);
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    MenuButton::invoke(&MenuButton::handleMouseEvent, event);
}