#ifndef TIMER_H
#define TIMER_H

#include <SDL_timer.h>
#include <meta.hpp>


/**
 * @see https://lazyfoo.net/tutorials/SDL/23_advanced_timers/index.php
 * @note Credits are important!
*/
class Timer final : public Singleton<Timer> {
    public:
        Timer() = default;
        ~Timer() = default;

        void start();
        void stop();
        void pause();
        void unpause();

        uint32_t getTicks();

        bool isStarted();
        bool isPaused();

    private:
        uint32_t startTicks = 0;
        uint32_t pausedTicks = 0;

        bool isStarted_ = false;
        bool isPaused_ = true;
};


#endif