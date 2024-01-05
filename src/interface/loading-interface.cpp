#include <interface.hpp>

#include <SDL.h>
#include <auxiliaries.hpp>


LoadingInterface::LoadingInterface() {
    LoadingMessage::instantiate(config::text::initializerLoadingMessage);
    LoadingProgressBar::instantiate(config::progress_bar::intiializerLoadingProgressBar);
}

void LoadingInterface::deinitialize() {
    LoadingMessage::deinitialize();
    LoadingProgressBar::deinitialize();
}

void LoadingInterface::render() const {
    renderBackground();
    renderComponents();
}

void LoadingInterface::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, config::color::offblack);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void LoadingInterface::renderComponents() const {
    LoadingMessage::invoke(&LoadingMessage::render);
    LoadingProgressBar::invoke(&LoadingProgressBar::render);
}

void LoadingInterface::onWindowChange() {
    LoadingMessage::invoke(&LoadingMessage::onWindowChange);
    LoadingProgressBar::invoke(&LoadingProgressBar::onWindowChange);
}

void LoadingInterface::updateAnimation() {
    LoadingProgressBar::invoke(&LoadingProgressBar::updateAnimation);
}

void LoadingInterface::initiateTransition(GameState const& gameState) {
    globals::state = GameState::kLoading;
    nextGameState = gameState;
    LoadingProgressBar::instance->isActivated = true;
}

void LoadingInterface::handleTransition() {
    if (!LoadingProgressBar::instance->isFinished) return;
    if (currIdleFrames < kIdleFramesLimit) ++currIdleFrames; else {
        currIdleFrames = 0;
        globals::state = nextGameState; 
        LoadingProgressBar::invoke(&LoadingProgressBar::resetProgress);
    }
}