#include <mixer.hpp>

#include <SDL_mixer.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


Mixer::Mixer() : Singleton<Mixer>() {
    BGMMenu = Mix_LoadMUS(config::mixer::MenuBGM.string().c_str());
    SFXButtonClick = Mix_LoadWAV(config::mixer::SFXButtonClick.string().c_str());
}

Mixer::~Mixer() {
    if (BGMMenu != nullptr) {
        Mix_FreeMusic(BGMMenu);
        BGMMenu = nullptr;
    }
    if (SFXButtonClick != nullptr) {
        Mix_FreeChunk(SFXButtonClick);
        SFXButtonClick = nullptr;
    }
}

void Mixer::playBGM() {
    if (Mix_PlayingMusic()) return;
    Mix_PlayMusic(BGMMenu, -1);
}

void Mixer::stopBGM() {
    Mix_HaltMusic();
}

void Mixer::pauseBGM() {
    if (!Mix_PlayingMusic() || Mix_PausedMusic()) return;
    Mix_PauseMusic();
}

void Mixer::unpauseBGM() {
    if (!Mix_PlayingMusic() || !Mix_PausedMusic()) return;
    Mix_ResumeMusic();
}

void Mixer::playSFX() {
    Mix_PlayChannel(-1, SFXButtonClick, 0);
}