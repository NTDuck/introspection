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
        void handleWindowEvent(const SDL_Event* event, SDL_Renderer* renderer);

        void setWindowSurface(SDL_Renderer* renderer);

        SDL_Window* window = nullptr;
        SDL_Surface* windowSurface = nullptr;
        Uint32 windowID;

        BackgroundHandler backgroundHandler;

    private:
        const Flags flags;
        Dimensions dimensions;
        std::string title;
};