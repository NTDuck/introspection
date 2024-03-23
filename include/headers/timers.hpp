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

        unsigned int getTicks() const;

        inline bool isStarted() const { return mIsStarted; }
        inline bool isPaused() const { return mIsPaused && mIsStarted; }

    protected:
        unsigned int mStartTicks = 0;
        unsigned int mPausedTicks = 0;

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
        static constexpr unsigned int kTicksPerFrame = 1000 / config::game::FPS;
};

class CountdownTimer final : public GenericTimer {
    public:
        CountdownTimer(unsigned int maxTicks = 1000) { setMaxTicks(maxTicks); }
        ~CountdownTimer() = default;

        inline bool isFinished() const { return getTicks() >= mMaxTicks; }
        inline void setMaxTicks(unsigned int maxTicks) { mMaxTicks = maxTicks; }

    private:
        unsigned int mMaxTicks;
};


#endif