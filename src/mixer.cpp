#include <mixer.hpp>

#include <SDL_mixer.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Mixer::Mixer() : Singleton<Mixer>() {
    for (auto& pair : kGameStateBGMMapping) if (pair.second.first == nullptr) pair.second.first = Mix_LoadMUS(pair.second.second.string().c_str());
    for (auto& pair : kLevelBGMMapping) if (pair.second.first == nullptr) pair.second.first = Mix_LoadMUS(pair.second.second.string().c_str());
    for (auto& pair : kSFXMapping) if (pair.second.first == nullptr) pair.second.first = Mix_LoadWAV(pair.second.second.string().c_str());
}

Mixer::~Mixer() {
    for (auto& pair : kGameStateBGMMapping) if (pair.second.first != nullptr) Mix_FreeMusic(pair.second.first);
    for (auto& pair : kLevelBGMMapping) if (pair.second.first != nullptr) Mix_FreeMusic(pair.second.first);
    for (auto& pair : kSFXMapping) if (pair.second.first != nullptr) Mix_FreeChunk(pair.second.first);
}

void Mixer::playBGM(Mix_Music* BGM) const {
    if constexpr(!config::enable_audio) return;
    if (Mix_PlayingMusic()) return;
    Mix_PlayMusic(BGM, -1);
}

void Mixer::stopBGM() const {
    if constexpr(!config::enable_audio) return;
    Mix_HaltMusic();
}

void Mixer::pauseBGM() const {
    if constexpr(!config::enable_audio) return;
    if (!Mix_PlayingMusic() || Mix_PausedMusic()) return;
    Mix_PauseMusic();
}

void Mixer::unpauseBGM() const {
    if constexpr(!config::enable_audio) return;
    if (!Mix_PlayingMusic() || !Mix_PausedMusic()) return;
    Mix_ResumeMusic();
}

void Mixer::playSFX(SFXName SFX) const {
    if constexpr(!config::enable_audio) return;
    auto it = kSFXMapping.find(SFX);
    if (it != kSFXMapping.end()) Mix_PlayChannel(-1, it->second.first, 0);
}

/**
 * @brief Change BGM upon entering new level.
 * @note Manually called.
*/
void Mixer::onLevelChange(level::Name newLevel) const {
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
    prevState = globals::state;   // Req this
}

void Mixer::onGameStateChange(GameState const prev, GameState const next) {
    if (prev == next) return;   // Prevent redundant calls

    switch (next) {
        case (GameState::kLoading | GameState::kIngamePlaying):
        case (GameState::kLoading | GameState::kMenu):
            return;   // Prevent calls on "arbitrary" states

        // Prevent overlap with `onLevelChange()`
        case GameState::kLoading: switch (prev) {
            case (GameState::kLoading | GameState::kIngamePlaying):
                // This is where `onLevelChange()` gets called
                return;

            default: break;
        } break;

        // Dialogues should not interrupt in-game BGM
        case GameState::kIngamePlaying: switch (prev) {
            case GameState::kLoading:
            case GameState::kIngameDialogue:
                return;
            
            default: break;
        } break;

        case GameState::kIngameDialogue: return;

        default: break;            
    }


    stopBGM();
    auto it = kGameStateBGMMapping.find(next);
    if (it != kGameStateBGMMapping.end()) playBGM(it->second.first);
}