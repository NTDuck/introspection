#include <iostream>
#include "../include/headers/game.h"


Game::Game() {
    flags.init = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    flags.window = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    dimensions._w = 1280;
    dimensions._h = 720;

    title = "8964";
    frameRate = 120;
};

Game::~Game() {
    if (window != nullptr) SDL_DestroyWindow(window);   // deallocate memory
    SDL_Quit();   // quit SDL subsystems
};

void Game::run() {
    init();
    gameLoop();
}

void Game::init() {
    SDL_Init(flags.init);
    window = SDL_CreateWindow(title, dimensions._x, dimensions._y, dimensions._w, dimensions._h, flags.window);
    windowID = SDL_GetWindowID(window);
    renderer = SDL_CreateRenderer(window, -1, flags.renderer);
}

void Game::gameLoop() {
    int last_ticks = SDL_GetTicks64();

    while (gamestate != State::EXIT) {
        if (SDL_GetTicks64() - last_ticks < 1000 / frameRate) continue;
        last_ticks = SDL_GetTicks64();
        handleEvents();
    }
}

void Game::handleEvents() {
    SDL_Event* event = new SDL_Event;
    if (!SDL_PollEvent(event)) return;

    switch (event -> type) {
        case SDL_QUIT:
            gamestate = State::EXIT;
            std::cout << "exit signal received, program terminated successfully." << std::endl;
            break;
        
        case SDL_WINDOWEVENT:
            handleWindowEvent(event);
        // also called when mouse focus regained/lost
        case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(event);
    }

    delete event;
}

void Game::handleWindowEvent(SDL_Event* event) {
    if (event -> window.windowID != windowID) return;
    switch (event -> window.event) {
        case SDL_WINDOWEVENT_MOVED:
            return;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
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

void Game::handleKeyBoardEvent(SDL_Event* event) {
    // apparently scancode denotes physical location, keycode denotes actual meaning (different if remapped)
    switch (event -> type) {
        case SDL_KEYDOWN:
            std::cout << "key-pressed: [" << SDL_GetKeyName(event -> key.keysym.sym) << "]" << std::endl;
            return;
    }
}

void Game::handleMouseEvent(SDL_Event* event) {
    switch (event -> type) {
        case SDL_MOUSEBUTTONDOWN:
            mouse.state = SDL_GetMouseState(&mouse._x, &mouse._y);
            std::cout << "mouse-down: (" << mouse._x << ", " << mouse._y << ")" << std::endl;
            return;
    }
}