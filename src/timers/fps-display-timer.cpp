#include <timers.hpp>

#include <SDL_timer.h>


void FPSDisplayTimer::calculateFPS() {
    averageFPS = accumulatedFrames / (static_cast<int>(getTicks()) / 1000.0f);
    if (accumulatedFrames >= std::numeric_limits<unsigned long long int>::max()) accumulatedFrames = 0;
    ++accumulatedFrames;
}