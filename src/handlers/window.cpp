#include <iostream>

#include <handlers/window.h>
#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


WindowHandler::WindowHandler(Flags flags, Dimensions dimensions, std::string title) : backgroundHandler(dimensions), flags(flags), dimensions(dimensions), title(title) {}

// deallocate memory
WindowHandler::~WindowHandler() {
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
    if (window != nullptr) SDL_DestroyWindow(window);
}

// called in Game::init()
void WindowHandler::init() {
    window = SDL_CreateWindow(title.c_str(), dimensions._x, dimensions._y, dimensions._w, dimensions._h, flags.window);
    windowID = SDL_GetWindowID(window);
}

// must be called after window is resized 
void WindowHandler::setWindowSurface(SDL_Renderer* renderer) {
    windowSurface = SDL_GetWindowSurface(window);
    backgroundHandler.changeBackground(renderer, static_cast<BackgroundType>(backgroundHandler.position));
}

void WindowHandler::handleWindowEvent(const SDL_Event* event, SDL_Renderer* renderer) {
    if (event -> window.windowID != windowID) return;
    switch (event -> window.event) {
        case SDL_WINDOWEVENT_MOVED:
            return;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            setWindowSurface(renderer);
            std::cout << "window-size-changed" << std::endl;
            return;
        case SDL_WINDOWEVENT_MINIMIZED:
            std::cout << "window-minimized" << std::endl;
            return;
        case SDL_WINDOWEVENT_MAXIMIZED:
            std::cout << "window-maximized" << std::endl;
            return;
        case SDL_WINDOWEVENT_ENTER:
            std::cout << "window-focus-mouse-gained" << std::endl;
            return;
        case SDL_WINDOWEVENT_LEAVE:
            std::cout << "window-focus-mouse-lost" << std::endl;
            return;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            std::cout << "window-focus-keyboard-gained" << std::endl;
            return;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            std::cout << "window-focus-keyboard-lost" << std::endl;
            return;
    }
}