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
    MenuAnimatedBackground::instantiate(IngameMapHandler::instance->mTexture, IngameMapHandler::instance->mTextureSize, globals::windowSize);
    // MenuAvatar::instantiate(*Teleporter::tilesetData, config::components::menu_avatar::destRectModifier);
    MenuButton::instantiate(config::components::menu_button::initializer);
    MenuTitle::instantiate(config::components::menu_title::initializer);
}

void MenuInterface::initialize() {
    MenuButton::initialize();
}

void MenuInterface::deinitialize() {
    MenuAnimatedBackground::deinitialize();
    // MenuAvatar::deinitialize();
    MenuButton::deinitialize();
    MenuTitle::deinitialize();

    Singleton<MenuInterface>::deinitialize();
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
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::render);
    // MenuAvatar::invoke(&MenuAvatar::render);
    MenuButton::invoke(&MenuButton::render);
    MenuTitle::invoke(&MenuTitle::render);
}

void MenuInterface::onWindowChange() {
    AbstractInterface<MenuInterface>::onWindowChange();
    
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::onWindowChange);
    // MenuAvatar::invoke(&MenuAvatar::onWindowChange);
    MenuButton::invoke(&MenuButton::onWindowChange);
    MenuTitle::invoke(&MenuTitle::onWindowChange);
}

void MenuInterface::updateAnimation() {
    MenuAnimatedBackground::invoke(&MenuAnimatedBackground::updateAnimation);
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    MenuButton::invoke(&MenuButton::handleMouseEvent, event);
    MenuButton::handleCursor();
}