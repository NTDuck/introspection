#include <mixer.hpp>

#include <SDL_mixer.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Mixer::Mixer() : Singleton<Mixer>() {
    for (auto& pair : kGameStateBGMMapping) if (pair.second.first == nullptr) pair.second.first = Mix_LoadMUS(pair.second.second.string().c_str());
    for (auto& pair : kLevelBGMMapping) if (pair.second.first == nullptr) pair.second.first = Mix_LoadMUS(pair.second.second.string().c_str());

    SFXButtonClick = Mix_LoadWAV(config::mixer::SFXButtonClick.string().c_str());
}

Mixer::~Mixer() {
    for (auto& pair : kGameStateBGMMapping) if (pair.second.first != nullptr) Mix_FreeMusic(pair.second.first);
    for (auto& pair : kLevelBGMMapping) if (pair.second.first != nullptr) Mix_FreeMusic(pair.second.first);

    if (SFXButtonClick != nullptr) {
        Mix_FreeChunk(SFXButtonClick);
        SFXButtonClick = nullptr;
    }
}

void Mixer::playBGM(Mix_Music* BGM) const {
    if (Mix_PlayingMusic()) return;
    Mix_PlayMusic(BGM, -1);
}

void Mixer::stopBGM() const {
    Mix_HaltMusic();
}

void Mixer::pauseBGM() const {
    if (!Mix_PlayingMusic() || Mix_PausedMusic()) return;
    Mix_PauseMusic();
}

void Mixer::unpauseBGM() const {
    if (!Mix_PlayingMusic() || !Mix_PausedMusic()) return;
    Mix_ResumeMusic();
}

void Mixer::playSFX() const {
    Mix_PlayChannel(-1, SFXButtonClick, 0);
}

/**
 * @brief Change BGM upon entering new level.
 * @note Manually called.
*/
void Mixer::onLevelChange(level::LevelName newLevel) {
    stopBGM();
    auto it = kLevelBGMMapping.find(newLevel);
    if (it != kLevelBGMMapping.end()) playBGM(it->second.first);
}

/**
 * @brief Change BGM upon entering new game state.
 * @note Works independently.
*/
void Mixer::handleGameStateChange() {
    static GameState prevState;   // Initialize with decoy value
    onGameStateChange(prevState, globals::state);
    prevState = globals::state;
}

void Mixer::onGameStateChange(GameState const prev, GameState const next) {
    switch (next) {
        case (GameState::kLoading | GameState::kIngamePlaying) : case (GameState::kLoading | GameState::kMenu): return;   // Prevent calls on "arbitrary" states

        // Prevent overlap with `onLevelChange()`
        case GameState::kLoading: if (prev == (GameState::kLoading | GameState::kIngamePlaying)) return;   // This is where `onLevelChange()` gets called
            break;
        case GameState::kIngamePlaying: if (prev == GameState::kLoading) return;
            break;

        default: break;
    }

    if (prev == next) return;   // Prevent redundant calls

    stopBGM();
    auto it = kGameStateBGMMapping.find(next);
    if (it != kGameStateBGMMapping.end()) playBGM(it->second.first);
}