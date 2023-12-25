#include <interface.hpp>

#include <unordered_map>
#include <unordered_set>

#include <meta.hpp>


/**
 * @brief Populate protected member `buttons`.
 * @note Might move to `globals::config` instead.
*/
MenuInterface::MenuInterface() {
    static const std::unordered_map<std::string, SDL_FPoint> initializer = {
        { "NEW GAME", { 1.0f / 3.0f, 7.0f / 9.0f } },
        { "CONTINUE", { 1.0f / 3.0f, 8.0f / 9.0f } },
        { "SETTINGS", { 2.0f / 3.0f, 7.0f / 9.0f } },
        { "ABOUT", { 2.0f / 3.0f, 8.0f / 9.0f } },
    };
    for (const auto& pair : initializer) GenericButton::instances.insert(new GenericButton(pair.first, pair.second));
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