#include <iostream>

#include <game.h>
#include <managers/window.h>


Game::Game() {
    flags.init = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    frameRate = 120;
};

Game::~Game() {
    windowManager.~WindowManager();
    SDL_Quit();   // quit SDL subsystems
};

void Game::run() {
    init();
    gameLoop();
}

void Game::init() {
    SDL_Init(flags.init);
    windowManager.init();
    renderer = SDL_CreateRenderer(windowManager.window, -1, flags.renderer);
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
            windowManager.handleWindowEvent(event);
        // also called when mouse focus regained/lost
        case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(event);
    }

    delete event;
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