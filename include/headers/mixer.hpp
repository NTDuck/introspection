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
            { GameState::kMenu, std::make_pair(nullptr, config::path::asset_audio / "bgm/dearly-departed.mp3") },
            { GameState::kGameOver, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-all-you-are-going-to-want-to-do-is-get-back-there.mp3") },
        };

        std::unordered_map<level::Name, std::pair<Mix_Music*, std::filesystem::path>> kLevelBGMMapping = {
            { level::Name::kLevelPrologueMovement, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-late-afternoon-drifting.mp3") },
            { level::Name::kLevelPrologueCombat, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-late-afternoon-drifting.mp3") },

            { level::Name::kLevelSuffering_empty, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-lacunar-amnesia.mp3") },
            { level::Name::kLevelSuffering_1_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-persistent-repetition-of-phrases.mp3") },
            { level::Name::kLevelSuffering_1_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-von-restorff-effect.mp3") },
            { level::Name::kLevelSuffering_2_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-false-memory-syndrome.mp3") },
            { level::Name::kLevelSuffering_2_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-unmasking-alzheimers.mp3") },
            { level::Name::kLevelSuffering_3_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-past-life-regression.mp3") },
            { level::Name::kLevelSuffering_3_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-rosy-retrospection.mp3") },
            { level::Name::kLevelSuffering_3_2, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-persistent-repetition-of-phrases.mp3") },

            { level::Name::kLevelStatusQuo_0, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-all-you-are-going-to-want-to-do-is-get-back-there.mp3") },
            { level::Name::kLevelStatusQuo_1, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-all-you-are-going-to-want-to-do-is-get-back-there.mp3") },
            { level::Name::kLevelStatusQuo_2, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-all-you-are-going-to-want-to-do-is-get-back-there.mp3") },
            { level::Name::kLevelStatusQuo_3, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-its-just-a-burning-memory.mp3") },
            { level::Name::kLevelStatusQuo_4, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-everything-is-on-the-point-of-decline.mp3") },

            { level::Name::kLevelRoundtable_empty, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-everything-is-on-the-point-of-decline.mp3") },
            { level::Name::kLevelRoundtable_first, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-everything-is-on-the-point-of-decline.mp3") },
            { level::Name::kLevelRoundtable_second, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-everything-is-on-the-point-of-decline.mp3") },
            { level::Name::kLevelRoundtable_final, std::make_pair(nullptr, config::path::asset_audio / "bgm/caretaker-everything-is-on-the-point-of-decline.mp3") },
            
            { level::Name::kLevelWhiteSpace, std::make_pair(nullptr, config::path::asset_audio / "bgm/omori-002-white-space.mp3") },
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