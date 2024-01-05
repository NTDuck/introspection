#include <components.hpp>

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
    currProgress += kProgressUpdateRate;

    if (currProgress > kProgressUpdateRateLimit) {
        currProgress = kProgressUpdateRateLimit;
        isFinished = true;
        isActivated = false;
    }

    progressDestRects.first.w = static_cast<int>(currProgress * shrinkedBoxDestRect.w);
    progressDestRects.second.x = shrinkedBoxDestRect.x + progressDestRects.first.w;
    progressDestRects.second.w = shrinkedBoxDestRect.w - progressDestRects.first.w;
}

template <typename T>
void GenericProgressBarComponent<T>::resetProgress() {
    currProgress = 0;
    isFinished = isActivated = false;
}


template class GenericProgressBarComponent<LoadingProgressBar>;