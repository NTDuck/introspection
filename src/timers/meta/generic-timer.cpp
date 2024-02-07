#include <timers.hpp>

#include <SDL_timer.h>


/**
 * @note This method can also be used to restart the timer.
*/
template <typename T>
void GenericTimer<T>::start() {
    mIsStarted = true;
    mIsPaused = false;

    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

template <typename T>
void GenericTimer<T>::stop() {
    mIsStarted = false;
    mIsPaused = true;

    mStartTicks = 0;
    mPausedTicks = 0;
}

template <typename T>
void GenericTimer<T>::pause() {
    if (!mIsStarted || mIsPaused) return;

    mIsPaused = true;
    mPausedTicks = SDL_GetTicks() - mStartTicks;   // The time the timer was paused based on `startTicks`
    mStartTicks = 0;
}

template <typename T>
void GenericTimer<T>::unpause() {
    if (!mIsStarted || !mIsPaused) return;

    mIsPaused = false;
    mStartTicks = SDL_GetTicks() - mPausedTicks;   // Continue based on previously recorded `pausedTicks`
    mPausedTicks = 0;
}

template <typename T>
uint32_t GenericTimer<T>::getTicks() {
    return mIsStarted ? (mIsPaused ? mPausedTicks : SDL_GetTicks() - mStartTicks) : 0;
}

template <typename T>
bool GenericTimer<T>::isStarted() {
    return mIsStarted;
}

template <typename T>
bool GenericTimer<T>::isPaused() {
    return mIsPaused && mIsStarted;
}


template class GenericTimer<FPSDisplayTimer>;
template class GenericTimer<FPSControlTimer>;