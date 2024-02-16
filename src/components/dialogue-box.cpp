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
            if (mStatus != Status::kUpdateComplete) break;
            mStatus = Status::kInactive;
            globals::state = GameState::kIngamePlaying;
            break;
            
        default: break;
    }
}

void IngameDialogueBox::updateContent() {
    if (mStatus != Status::kUpdateInProgress) return;

    // Really funny things gonna happen here
    std::cout << mContent[mCurrProgress] << std::endl;

    if (mCurrProgress == static_cast<unsigned short int>(mContent.size()) - 1) mStatus = Status::kUpdateComplete; else ++mCurrProgress;
}

void IngameDialogueBox::editContent(std::string const& content) {
    if (mStatus != Status::kInactive || content.empty()) return;

    mStatus = Status::kUpdateInProgress;
    mCurrProgress = 0;
    mContent = content;

    globals::state = GameState::kIngameDialogue;
}


template <>
const double GenericComponent<IngameDialogueBox>::kDestSizeModifier = config::components::dialogue_box::destSizeModifier;

template <>
const SDL_Point GenericComponent<IngameDialogueBox>::kDestRectRatio = config::components::dialogue_box::destRectRatio;