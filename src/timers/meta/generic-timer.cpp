#include <timers.hpp>

#include <SDL_timer.h>


/**
 * @note This method can also be used to restart the timer.
*/
template <typename T>
void GenericTimer<T>::start() {
    isStarted_ = true;
    isPaused_ = false;

    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

template <typename T>
void GenericTimer<T>::stop() {
    isStarted_ = false;
    isPaused_ = true;

    startTicks = 0;
    pausedTicks = 0;
}

template <typename T>
void GenericTimer<T>::pause() {
    if (!isStarted_ || isPaused_) return;

    isPaused_ = true;
    pausedTicks = SDL_GetTicks() - startTicks;   // The time the timer was paused based on `startTicks`
    startTicks = 0;
}

template <typename T>
void GenericTimer<T>::unpause() {
    if (!isStarted_ || !isPaused_) return;

    isPaused_ = false;
    startTicks = SDL_GetTicks() - pausedTicks;   // Continue based on previously recorded `pausedTicks`
    pausedTicks = 0;
}

template <typename T>
uint32_t GenericTimer<T>::getTicks() {
    return isStarted_ ? (isPaused_ ? pausedTicks : SDL_GetTicks() - startTicks) : 0;
}

template <typename T>
bool GenericTimer<T>::isStarted() {
    return isStarted_;
}

template <typename T>
bool GenericTimer<T>::isPaused() {
    return isPaused_ && isStarted_;
}


template class GenericTimer<FPSDisplayTimer>;
template class GenericTimer<FPSControlTimer>;