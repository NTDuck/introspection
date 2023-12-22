#include <interface.hpp>

#include <unordered_set>

#include <meta.hpp>


/**
 * @brief Populate protected member `buttons`.
 * @note Might move to `globals::config` instead.
*/
MenuInterface::MenuInterface() {
    buttons.insert(new Division("NEW GAME", { 1.0f / 3.0f, 7.0f / 9.0f }));
    buttons.insert(new Division("CONTINUE", { 1.0f / 3.0f, 8.0f / 9.0f }));
    buttons.insert(new Division("SETTINGS", { 2.0f / 3.0f, 7.0f / 9.0f }));
    buttons.insert(new Division("ABOUT", { 2.0f / 3.0f, 8.0f / 9.0f }));
}

void MenuInterface::initialize() {
    Division::initialize();
}

void MenuInterface::deinitialize() {
    Division::deinitialize();
}

void MenuInterface::render() {
    renderBackground();
    renderComponents();
}

/**
 * @note In future commits, this method will take advantage of `texture`.
*/
void MenuInterface::renderBackground() {
    utils::setRendererDrawColor(globals::renderer, globals::config::kDefaultBackgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void MenuInterface::renderComponents() {
    for (auto& button : buttons) button->render();
}

void MenuInterface::onWindowChange() {
    for (auto& button : buttons) button->onWindowChange();
}