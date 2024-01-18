#ifndef MIXER_H
#define MIXER_H

#include <unordered_map>

#include <SDL.h>
#include <SDL_mixer.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Responsible for all operations related to music & sound effects.
*/
class Mixer final : Singleton<Mixer> {
    public:
        INCL_SINGLETON(Mixer)

        Mixer();
        ~Mixer();

        void playBGM(Mix_Music* BGM) const;
        void stopBGM() const;
        void pauseBGM() const;
        void unpauseBGM() const;

        void playSFX() const;

        void onLevelChange(level::LevelName newLevel);
        void handleGameStateChange();

    private:
        void onGameStateChange(GameState const prev, GameState const next);

        /**
         * @note Should not include in-game states, as those should be governed by `kLevelBGMMapping`.
        */
        std::unordered_map<GameState, std::pair<Mix_Music*, std::filesystem::path>> kGameStateBGMMapping = {
            { GameState::kMenu, std::make_pair(nullptr, config::path::asset_audio / "bgm/animal-crossing-new-horizons-main-theme.mp3") },
            // { GameState::kLoading, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-013-a-home-for-flowers-tulip.mp3") },
        };

        std::unordered_map<level::LevelName, std::pair<Mix_Music*, std::filesystem::path>> kLevelBGMMapping = {
            { level::LevelName::kLevelEquilibrium, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-012-home.mp3") },
            { level::LevelName::kLevelValleyOfDespair, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-063-its-raining-somewhere-else.mp3") },
        };

        Mix_Chunk* SFXButtonClick = nullptr;
};


#endif