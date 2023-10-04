#include <iostream>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <game.h>
#include <handlers/window.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


Game::Game(Flags flags, Dimensions dimensions, unsigned short int frameRate, std::string title) : windowHandler(flags, dimensions, title), backgroundHandler(windowHandler.backgroundHandler), flags(flags), dimensions(dimensions), frameRate(frameRate), title(title) {
    gameState = GameState::PLAYING;
};

// quit SDL subsystems
Game::~Game() {
    IMG_Quit();
    SDL_Quit();
};

void Game::start() {
    init();
    gameLoop();
}

void Game::init() {
    if (SDL_Init(flags.init) < 0) std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
    if (!(IMG_Init(flags.image) & flags.image)) std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;   // https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/index2.php

    windowHandler.init();
}

void Game::gameLoop() {
    int last_ticks = SDL_GetTicks64();

    while (gameState != GameState::EXIT) {
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
            gameState = GameState::EXIT;
            std::cout << "exit signal received, program terminated successfully." << std::endl;
            break;
        
        case SDL_WINDOWEVENT:
            windowHandler.handleWindowEvent(event);
            break;
        // also called when mouse focus regained/lost
        case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
            break;
        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(event);
            break;
    }

    delete event;
}

void Game::handleKeyBoardEvent(SDL_Event* event) {
    // apparently scancode denotes physical location, keycode denotes actual meaning (different if remapped)
    switch (event -> type) {
        case SDL_KEYDOWN:
            std::cout << "key-pressed: [" << SDL_GetKeyName(event -> key.keysym.sym) << "]" << std::endl;

            switch (event -> key.keysym.sym) {
                case SDLK_w:   // reset to original
                    backgroundHandler.position = BackgroundType::curr;
                    break;
                case SDLK_q:   // move westwards
                    backgroundHandler.position -= (backgroundHandler.position > 0 ? 1 : 0);
                    SDL_UpdateWindowSurface(windowHandler.window);
                    break;
                case SDLK_e:
                    backgroundHandler.position += (backgroundHandler.position < 2 ? 1 : 0);
                    break;
                default:
                    return;
            }

            backgroundHandler.changeBackground(windowHandler.windowSurface, static_cast<BackgroundType>(backgroundHandler.position), windowHandler.windowSurface -> format);
            SDL_UpdateWindowSurface(windowHandler.window);

            break;
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