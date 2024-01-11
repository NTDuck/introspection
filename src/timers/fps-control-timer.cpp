#include <timers.hpp>

#include <SDL_timer.h>


void FPSControlTimer::controlFPS() {
    int ticks = static_cast<int>(getTicks());
    if (ticks < kTicksPerFrame) SDL_Delay(kTicksPerFrame - ticks);
}