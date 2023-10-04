#pragma once

#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


class WindowHandler {
    public:
        WindowHandler(Flags flags, Dimensions dimensions, std::string title);
        ~WindowHandler();

        void init();
        void handleWindowEvent(const SDL_Event* event);

        void setWindowSurface();

        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;
        Uint32 windowID;

        SDL_Renderer* renderer = nullptr;

        BackgroundHandler backgroundHandler;

    private:
        const Flags flags;
        Dimensions dimensions;
        std::string title;
};