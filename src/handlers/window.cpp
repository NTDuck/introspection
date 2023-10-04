#include <iostream>

#include <handlers/window.h>
#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


WindowHandler::WindowHandler(Flags flags, Dimensions dimensions, std::string title) : backgroundHandler(dimensions), flags(flags), dimensions(dimensions), title(title) {}

// deallocate memory
WindowHandler::~WindowHandler() {
    if (window != nullptr) SDL_DestroyWindow(window);
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
}

// called in Game::init()
void WindowHandler::init() {
    window = SDL_CreateWindow(title.c_str(), dimensions._x, dimensions._y, dimensions._w, dimensions._h, flags.window);
    windowID = SDL_GetWindowID(window);
    renderer = SDL_CreateRenderer(this -> window, -1, flags.renderer);
    setWindowSurface();
}

// must be called after window is resized 
void WindowHandler::setWindowSurface() {
    windowSurface = SDL_GetWindowSurface(window);
    backgroundHandler.changeBackground(windowSurface, static_cast<BackgroundType>(backgroundHandler.position), windowSurface -> format);
    SDL_UpdateWindowSurface(window);
}

void WindowHandler::handleWindowEvent(const SDL_Event* event) {
    if (event -> window.windowID != windowID) return;
    switch (event -> window.event) {
        case SDL_WINDOWEVENT_MOVED:
            return;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            setWindowSurface();
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