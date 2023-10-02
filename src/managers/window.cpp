#include <iostream>
#include <managers/window.h>


WindowManager::WindowManager() {
    dimensions._w = 1280;
    dimensions._h = 720;

    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    title = "8964";
}

// deallocate memory
WindowManager::~WindowManager() {
    if (window != nullptr) SDL_DestroyWindow(window);
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
}

// called in Game::init()
void WindowManager::init() {
    window = SDL_CreateWindow(title, dimensions._x, dimensions._y, dimensions._w, dimensions._h, flags);
    windowID = SDL_GetWindowID(window);

    setWindowSurface();
    std::cout << SDL_BlitSurface(background.surface, NULL, windowSurface, NULL) << std::endl;   // if -1 something is definitely wrong
    SDL_UpdateWindowSurface(window);   // called twice unfortunately
}

// must be called after window is resized 
void WindowManager::setWindowSurface() {
    windowSurface = SDL_GetWindowSurface(window);
    // SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface -> format, 0x69, 0x69, 0x69));   // fill the surface with something, unnecessary anyway
    SDL_UpdateWindowSurface(window);
}

void WindowManager::handleWindowEvent(SDL_Event* event) {
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