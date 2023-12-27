#include <interface.hpp>

#include <meta.hpp>


/**
 * @brief Populate protected member `buttons`.
 * @note Might move to `globals::config` instead.
*/
MenuInterface::MenuInterface() {
    GenericButton::instances.insert(new GenericButton(new GameState(GameState::kIngamePlaying), "NEW GAME", { 1.0f / 3.0f, 7.0f / 9.0f }));
    GenericButton::instances.insert(new GenericButton(nullptr, "CONTINUE", { 1.0f / 3.0f, 8.0f / 9.0f }));
    GenericButton::instances.insert(new GenericButton(nullptr, "SETTINGS", { 2.0f / 3.0f, 7.0f / 9.0f }));
    GenericButton::instances.insert(new GenericButton(nullptr, "ABOUT", { 2.0f / 3.0f, 8.0f / 9.0f }));
}

void MenuInterface::initialize() {
    GenericButton::initialize();
}

void MenuInterface::deinitialize() {
    GenericButton::deinitialize();
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
    GenericButton::callOnEach(&GenericButton::render);
}

void MenuInterface::onWindowChange() {
    GenericButton::callOnEach(&GenericButton::onWindowChange);
}

void MenuInterface::handleMouseEvent(SDL_Event const& event) {
    GenericButton::callOnEach(&GenericButton::handleMouseEvent, event);
}