#include <components.hpp>

#include <filesystem>

#include <SDL.h>
#include <auxiliaries.hpp>


ExitText::ExitText(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content) : GenericComponent<ExitText>(center, preset), GenericTextComponent<ExitText>(center, preset, content) {}

void ExitText::deinitialize() {
    Singleton<ExitText>::deinitialize();
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

void ExitText::render() const {
    if (!mCurrProgress) return;
    GenericTextComponent<ExitText>::render();
}

void ExitText::onWindowChange() {
    GenericTextComponent<ExitText>::onWindowChange();
    registerProgress();
}

void ExitText::handleKeyBoardEvent(SDL_Event const& event) {
    if (event.key.keysym.sym != config::key::EXIT || event.type != SDL_KEYDOWN) {
        // Reset
        mCurrProgress = 0;
        return;
    }

    mCurrProgress += kProgressUpdateRate;
    registerProgress();
    if (mCurrProgress < kProgressUpdateRateLimit) return;

    // Exit
    mCurrProgress = kProgressUpdateRateLimit;
    globals::state = GameState::kExit;
}

/**
 * @brief Set transparency of `textTexture`.
 * @see <src/components/generic-progress-bar.cpp> GenericProgressBarComponent<T>::updateAnimation()
*/
void ExitText::registerProgress() {
    auto eq = [](double progress) {
        return std::sqrt(1.0 - pow(progress - 1.0, 2));
    };
    SDL_SetTextureAlphaMod(textTexture, SDL_ALPHA_OPAQUE * eq(mCurrProgress));
}


template <>
const double GenericComponent<ExitText>::kDestSizeModifier = config::components::exit_text::destSizeModifier;

template <>
const SDL_Point GenericComponent<ExitText>::kDestRectRatio = config::components::exit_text::destRectRatio;

template <>
const std::filesystem::path GenericTextComponent<ExitText>::fontPath = config::components::exit_text::fontPath;