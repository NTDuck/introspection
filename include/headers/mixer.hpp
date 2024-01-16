#ifndef MIXER_H
#define MIXER_H

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

        void playBGM();
        void stopBGM();
        void pauseBGM();
        void unpauseBGM();

        void playSFX();

    private:
        Mix_Music* BGMMenu = nullptr;
        Mix_Chunk* SFXButtonClick = nullptr;
};


#endif