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
    SDL_RenderFillRect(globals::renderer, &mProgressDestRects.first);

    utils::setRendererDrawColor(globals::renderer, kPreset.backgroundColor);
    SDL_RenderFillRect(globals::renderer, &mProgressDestRects.second);
}

template <typename T>
void GenericProgressBarComponent<T>::onWindowChange() {
    GenericBoxComponent<T>::onWindowChange();

    mShrinkedBoxDestRect = mBoxDestRect;
    shrinkRect(mShrinkedBoxDestRect, kPreset.lineWidth);

    mProgressDestRects.first = mProgressDestRects.second = mShrinkedBoxDestRect;
}

template <typename T>
void GenericProgressBarComponent<T>::updateAnimation() {
    if (!isActivated || isFinished) return;

    // Increase `mDecoyProgress` linearly
    mDecoyProgress += kProgressUpdateRate;
    if (mDecoyProgress > kProgressUpdateRateLimit) {
        mDecoyProgress = kProgressUpdateRateLimit;
        isFinished = true;
        isActivated = false;
    }
    
    // Calculate `mCurrProgress` based on `mDecoyProgress`
    auto eq = [](double x) {
        // Imagine `mDecoyProgress` and `mCurrProgress` being the x-axis and y-axis, respectively, of a Cartesian coordinate system
        return std::sqrt(1.0 - pow(x - 1.0, 2));   // Equation of circle `C((1, 0), R = 1)`
        // return 1.0 * std::cos(M_PI / 5 * x - M_PI / 2);   // Harmonic motion with `A = 1.0`, `ω = π/5`, `φ = -π/2`
    };
    mCurrProgress = eq(mDecoyProgress);

    mProgressDestRects.first.w = static_cast<int>(mCurrProgress * mShrinkedBoxDestRect.w);
    mProgressDestRects.second.x = mShrinkedBoxDestRect.x + mProgressDestRects.first.w;
    mProgressDestRects.second.w = mShrinkedBoxDestRect.w - mProgressDestRects.first.w;
}

template <typename T>
void GenericProgressBarComponent<T>::resetProgress() {
    mCurrProgress = mDecoyProgress = 0;
    isFinished = isActivated = false;
}


template class GenericProgressBarComponent<LoadingProgressBar>;