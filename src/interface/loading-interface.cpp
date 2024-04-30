#include <interface.hpp>

#include <SDL.h>

#include <components.hpp>
#include <auxiliaries.hpp>


LoadingInterface::LoadingInterface() {
    LoadingMessage::instantiate(config::components::loading_message::initializer);
    LoadingProgressBar::instantiate(config::components::loading_progress_bar::intializer);
}

void LoadingInterface::deinitialize() {
    LoadingMessage::deinitialize();
    LoadingProgressBar::deinitialize();

    Singleton<LoadingInterface>::deinitialize();
}

void LoadingInterface::render() const {
    renderBackground();
    renderComponents();
}

void LoadingInterface::renderBackground() const {
    utils::setRendererDrawColor(globals::renderer, config::color::darker);
    SDL_RenderFillRect(globals::renderer, nullptr);
}

void LoadingInterface::renderComponents() const {
    LoadingMessage::invoke(&LoadingMessage::render);
    LoadingProgressBar::invoke(&LoadingProgressBar::render);
}

void LoadingInterface::onWindowChange() {
    AbstractInterface<LoadingInterface>::onWindowChange();

    LoadingMessage::invoke(&LoadingMessage::onWindowChange);
    LoadingProgressBar::invoke(&LoadingProgressBar::onWindowChange);
}

void LoadingInterface::updateAnimation() {
    LoadingProgressBar::invoke(&LoadingProgressBar::updateAnimation);
}

void LoadingInterface::initiateTransition(GameState const& gameState) {
    globals::state = GameState::kLoading;
    mNextGameState = gameState;
    LoadingProgressBar::instance->isActivated = true;
}

void LoadingInterface::handleTransition() {
    if (!LoadingProgressBar::instance->isFinished) return;
    if (mCurrIdleFrames < kIdleFramesLimit) ++mCurrIdleFrames; else {
        mCurrIdleFrames = 0;
        globals::state = mNextGameState;
        LoadingProgressBar::invoke(&LoadingProgressBar::resetProgress);
    }
}