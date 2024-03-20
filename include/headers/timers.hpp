#ifndef TIMER_H
#define TIMER_H

#include <SDL_timer.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/* Abstract templates */

/**
 * @brief Represent a generic timer. Provides common utilities.
 * @see https://lazyfoo.net/tutorials/SDL/23_advanced_timers/index.php
 * @note Credits are important!
*/
class GenericTimer {
    public:
        GenericTimer() = default;
        virtual ~GenericTimer() = default;

        void start();
        void stop();
        void pause();
        void unpause();

        uint32_t getTicks();

        inline bool isStarted() { return mIsStarted; }
        inline bool isPaused() { return mIsPaused && mIsStarted; }

    protected:
        uint32_t mStartTicks = 0;
        uint32_t mPausedTicks = 0;

        bool mIsStarted = false;
        bool mIsPaused = true;
};


/* Derived implementations */

/**
 * @brief Represent a timer tailored for calculating the average FPS.
*/
class FPSDisplayTimer final : public Singleton<FPSDisplayTimer>, public GenericTimer {
    public:
        INCL_SINGLETON(FPSDisplayTimer)

        FPSDisplayTimer() = default;
        ~FPSDisplayTimer() = default;

        void calculateFPS();

        double mAverageFPS;
        unsigned long long int mAccumulatedFrames;
};


/**
 * @brief Represent a timer tailored for capping in-game FPS.
*/
class FPSControlTimer final : public Singleton<FPSControlTimer>, public GenericTimer {
    public:
        INCL_SINGLETON(FPSControlTimer)
        
        FPSControlTimer() = default;
        ~FPSControlTimer() = default;

        void controlFPS();

    private:
        static constexpr int kTicksPerFrame = 1000 / config::game::FPS;
};


#endif