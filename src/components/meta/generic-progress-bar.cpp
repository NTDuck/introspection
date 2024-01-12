#include <components.hpp>

// #define _USE_MATH_DEFINES
#include <cmath>

#include <SDL.h>

#include <auxiliaries.hpp>


template <typename T>
GenericProgressBarComponent<T>::GenericProgressBarComponent(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<T>(center, preset), GenericBoxComponent<T>(center, preset) {}


template <typename T>
void GenericProgressBarComponent<T>::render() const {
    GenericBoxComponent<T>::render();

    utils::setRendererDrawColor(globals::renderer, kPreset.lineColor);
    SDL_RenderFillRect(globals::renderer, &progressDestRects.first);

    utils::setRendererDrawColor(globals::renderer, kPreset.backgroundColor);
    SDL_RenderFillRect(globals::renderer, &progressDestRects.second);
}

template <typename T>
void GenericProgressBarComponent<T>::onWindowChange() {
    GenericBoxComponent<T>::onWindowChange();

    shrinkedBoxDestRect = boxDestRect;
    shrinkRect(shrinkedBoxDestRect, kPreset.lineWidth);

    progressDestRects.first = progressDestRects.second = shrinkedBoxDestRect;
}

template <typename T>
void GenericProgressBarComponent<T>::updateAnimation() {
    if (!isActivated || isFinished) return;

    // Increase `decoyProgress` linearly
    decoyProgress += kProgressUpdateRate;
    if (decoyProgress > kProgressUpdateRateLimit) {
        decoyProgress = kProgressUpdateRateLimit;
        isFinished = true;
        isActivated = false;
    }
    
    // Calculate `currProgress` based on `decoyProgress`
    auto eq = [](double x) {
        // Imagine `decoyProgress` and `currProgress` being the x-axis and y-axis, respectively, of a Cartesian coordinate system
        return std::sqrt(1.0 - pow(x - 1.0, 2));   // Equation of circle `C((1, 0), R = 1)`
        // return 1.0 * std::cos(M_PI / 5 * x - M_PI / 2);   // Harmonic motion with `A = 1.0`, `ω = π/5`, `φ = -π/2`
    };
    currProgress = eq(decoyProgress);

    progressDestRects.first.w = static_cast<int>(currProgress * shrinkedBoxDestRect.w);
    progressDestRects.second.x = shrinkedBoxDestRect.x + progressDestRects.first.w;
    progressDestRects.second.w = shrinkedBoxDestRect.w - progressDestRects.first.w;
}

template <typename T>
void GenericProgressBarComponent<T>::resetProgress() {
    currProgress = decoyProgress = 0;
    isFinished = isActivated = false;
}


template class GenericProgressBarComponent<LoadingProgressBar>;