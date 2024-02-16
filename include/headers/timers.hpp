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
template <typename T>
class GenericTimer : public Singleton<T> {
    public:
        INCL_SINGLETON(T)

        GenericTimer() = default;
        virtual ~GenericTimer() = default;

        void start();
        void stop();
        void pause();
        void unpause();

        uint32_t getTicks();

        bool isStarted();
        bool isPaused();

    protected:
        uint32_t mStartTicks = 0;
        uint32_t mPausedTicks = 0;

        bool mIsStarted = false;
        bool mIsPaused = true;
};

#define INCL_GENERIC_TIMER(T) using GenericTimer<T>::start, GenericTimer<T>::stop, GenericTimer<T>::pause, GenericTimer<T>::unpause, GenericTimer<T>::getTicks, GenericTimer<T>::isStarted, GenericTimer<T>::isPaused, GenericTimer<T>::mStartTicks, GenericTimer<T>::mPausedTicks, GenericTimer<T>::mIsStarted, GenericTimer<T>::mIsPaused;


/* Derived implementations */

/**
 * @brief Represent a timer tailored for calculating the average FPS.
*/
class FPSDisplayTimer final : public GenericTimer<FPSDisplayTimer> {
    public:
        INCL_GENERIC_TIMER(FPSDisplayTimer)
        
        FPSDisplayTimer() = default;
        ~FPSDisplayTimer() = default;

        void calculateFPS();

        double mAverageFPS;
        unsigned long long int mAccumulatedFrames;
};


/**
 * @brief Represent a timer tailored for capping in-game FPS.
*/
class FPSControlTimer final : public GenericTimer<FPSControlTimer> {
    public:
        INCL_GENERIC_TIMER(FPSControlTimer)

        FPSControlTimer() = default;
        ~FPSControlTimer() = default;

        void controlFPS();

    private:
        static constexpr int kTicksPerFrame = 1000 / config::game::FPS;
};


#endif