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
        enum class SFXName : unsigned char {
            kDialogue,
            kMewo,
            kButtonClick,
            kSurgeAttack,
            kPlayerWalk,
            kPlayerRun,
            kPlayerAttack,
            kEntityAttack,
            kEntityDamaged,
            kEntityDeath,
            kPlayerDeath,
        };

        INCL_SINGLETON(Mixer)

        Mixer();
        ~Mixer();

        int getMasterVolume() const;
        int getBGMVolume() const;
        int getSFXVolume() const;

        void setMasterVolume(int volume) const;
        void setBGMVolume(int volume) const;
        void setSFXVolume(int volume) const;

        void playBGM(Mix_Music* BGM) const;
        void stopBGM() const;
        void pauseBGM() const;
        void unpauseBGM() const;

        void playSFX(SFXName SFX) const;
        void stopSFX() const;

        void onLevelChange(level::Name newLevel) const;
        void handleGameStateChange();

    private:
        void onGameStateChange(GameState const prev, GameState const next);

        /**
         * @note Should not include in-game states, as those should be governed by `kLevelBGMMapping`.
        */
        std::unordered_map<GameState, std::pair<Mix_Music*, std::filesystem::path>> kGameStateBGMMapping = {
            { GameState::kMenu, std::make_pair(nullptr, config::path::asset_audio / "bgm/nso-16-no-salvation-no-angels.mp3") },
            { GameState::kGameOver, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-011-determination.mp3") },
            // { GameState::kLoading, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-013-a-home-for-flowers-tulip.mp3") },
        };

        std::unordered_map<level::Name, std::pair<Mix_Music*, std::filesystem::path>> kLevelBGMMapping = {
            { level::Name::kLevelWhiteSpace, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-002-white-space.mp3") },
            { level::Name::kLevelBegin, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-088-lost-library.mp3") },
            { level::Name::kLevelForest_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-038-space-boyfriend-tape.mp3") },
            { level::Name::kLevelForest_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-038-space-boyfriend-tape.mp3") },
            { level::Name::kLevelForest_2, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-039-trouble-never-always.mp3") },
            { level::Name::kLevelForest_3, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-040-you-were-wrong-go-back.mp3") },
            { level::Name::kLevelForest_4, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-037-the-elegance-of-the-mysterious-and-distant.mp3") },
            { level::Name::kLevelDeprecatedTutorial_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-005-ruins.mp3") },
            { level::Name::kLevelDeprecatedTutorial_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-005-ruins.mp3") },
            { level::Name::__kLevelTutorial_0_Combat__, std::make_pair(nullptr, config::path::asset_audio / "bgm/undertale-009-enemy-approaching.mp3") },
        };

        std::unordered_map<SFXName, std::pair<Mix_Chunk*, std::filesystem::path>> kSFXMapping = {
            { SFXName::kDialogue, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-talking.mp3") },
            { SFXName::kMewo, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-mewo.mp3") },
            { SFXName::kButtonClick, std::make_pair(nullptr, config::path::asset_audio / "sfx/一般の警告音.mp3") },
            { SFXName::kSurgeAttack, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-beep.mp3") },
            { SFXName::kPlayerWalk, std::make_pair(nullptr, config::path::asset_audio / "sfx/zapsplat-foley-footstep-single-wet-ground-light-puddles.mp3") },
            { SFXName::kPlayerRun, std::make_pair(nullptr, config::path::asset_audio / "sfx/zapsplat-foley-footstep-single-boot-heavy-clump-step-gravel.mp3") },            
            { SFXName::kPlayerAttack, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-omori-attack.mp3") },
            { SFXName::kEntityAttack, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-swish.mp3") },
            { SFXName::kEntityDamaged, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-bite.mp3") },
            { SFXName::kEntityDeath, std::make_pair(nullptr, config::path::asset_audio / "sfx/wilhelm-scream.mp3") },
            { SFXName::kPlayerDeath, std::make_pair(nullptr, config::path::asset_audio / "sfx/omori-death-by-experiment-667.mp3") },
        };
};


#endif