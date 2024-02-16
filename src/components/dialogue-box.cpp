#include <components.hpp>

#include <SDL.h>

#include <auxiliaries.hpp>


IngameDialogueBox::IngameDialogueBox(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<IngameDialogueBox>(center, preset), GenericBoxComponent<IngameDialogueBox>(center, preset) {}

void IngameDialogueBox::render() const {
    GenericBoxComponent<IngameDialogueBox>::render();   // For now
}

void IngameDialogueBox::onWindowChange() {
    GenericBoxComponent<IngameDialogueBox>::onWindowChange();   // For now
}

void IngameDialogueBox::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case ~config::Key::kPlayerInteract:
            if (!isFinished()) return;
            globals::state = GameState::kIngamePlaying;
            break;
            
        default: break;
    }
}

void IngameDialogueBox::updateContent() {
    if (isFinished()) return;

    // Really funny things gonna happen here
    std::cout << mContent[mCurrProgress] << std::endl;

    ++mCurrProgress;
}

void IngameDialogueBox::editContent(std::string const& content) {
    if (!isFinished()) return;
    globals::state = GameState::kIngameDialogue;
    mCurrProgress = 0;
    mContent = content;
}


template <>
const double GenericComponent<IngameDialogueBox>::kDestSizeModifier = config::components::dialogue_box::destSizeModifier;

template <>
const SDL_Point GenericComponent<IngameDialogueBox>::kDestRectRatio = config::components::dialogue_box::destRectRatio;