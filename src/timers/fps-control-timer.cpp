#include <timers.hpp>

#include <SDL_timer.h>


void FPSControlTimer::controlFPS() {
    auto ticks = getTicks();
    if (ticks < kTicksPerFrame) SDL_Delay(kTicksPerFrame - ticks);
}