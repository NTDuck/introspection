#include <timer.hpp>

#include <SDL_timer.h>


/**
 * @note This method can also be used to restart the timer.
*/
void Timer::start() {
    isStarted_ = true;
    isPaused_ = false;

    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

void Timer::stop() {
    isStarted_ = false;
    isPaused_ = true;

    startTicks = 0;
    pausedTicks = 0;
}

void Timer::pause() {
    if (!isStarted_ || isPaused_) return;

    isPaused_ = true;
    pausedTicks = SDL_GetTicks() - startTicks;   // The time the timer was paused based on `startTicks`
    startTicks = 0;
}

void Timer::unpause() {
    if (!isStarted_ || !isPaused_) return;

    isPaused_ = false;
    startTicks = SDL_GetTicks() - pausedTicks;   // Continue based on previously recorded `pausedTicks`
    pausedTicks = 0;
}

uint32_t Timer::getTicks() {
    return isStarted_ ? (isPaused_ ? pausedTicks : SDL_GetTicks() - startTicks) : 0;
}

bool Timer::isStarted() {
    return isStarted_;
}

bool Timer::isPaused() {
    return isPaused_ && isStarted_;
}