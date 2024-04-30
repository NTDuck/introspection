#include <interface.hpp>

#include <components.hpp>


GameOverInterface::GameOverInterface() {
    GameOverTitle::instantiate(config::components::game_over_title::initializer);
    GameOverButton::instantiate(config::components::game_over_button::initializer);
}

void GameOverInterface::initialize() {
    GameOverButton::initialize();
}

void GameOverInterface::deinitialize() {
    GameOverTitle::deinitialize();
    GameOverButton::deinitialize();

    Singleton<GameOverInterface>::deinitialize();
}

void GameOverInterface::render() const {
    renderBackground();
    renderComponents();
}

void GameOverInterface::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, config::color::darker);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void GameOverInterface::renderComponents() const {
    GameOverTitle::invoke(&GameOverTitle::render);
    GameOverButton::invoke(&GameOverButton::render);
}

void GameOverInterface::onWindowChange() {
    AbstractInterface<GameOverInterface>::onWindowChange();

    GameOverTitle::invoke(&GameOverTitle::onWindowChange);
    GameOverButton::invoke(&GameOverButton::onWindowChange);
}

void GameOverInterface::handleMouseEvent(SDL_Event const& event) {
    GameOverButton::invoke(&GameOverButton::handleMouseEvent, event);
    GameOverButton::handleCursor();
}