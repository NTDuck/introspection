#include <timers.hpp>

#include <SDL_timer.h>


/**
 * @note This method can also be used to restart the timer.
*/
void GenericTimer::start() {
    mIsStarted = true;
    mIsPaused = false;

    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void GenericTimer::stop() {
    mIsStarted = false;
    mIsPaused = true;

    mStartTicks = 0;
    mPausedTicks = 0;
}

void GenericTimer::pause() {
    if (!mIsStarted || mIsPaused) return;

    mIsPaused = true;
    mPausedTicks = SDL_GetTicks() - mStartTicks;   // The time the timer was paused based on `startTicks`
    mStartTicks = 0;
}

void GenericTimer::unpause() {
    if (!mIsStarted || !mIsPaused) return;

    mIsPaused = false;
    mStartTicks = SDL_GetTicks() - mPausedTicks;   // Continue based on previously recorded `pausedTicks`
    mPausedTicks = 0;
}

unsigned int GenericTimer::getTicks() const {
    return mIsStarted ? (mIsPaused ? mPausedTicks : SDL_GetTicks() - mStartTicks) : 0;
}