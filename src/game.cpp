#include <iostream>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <game.h>
#include <characters/player.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


Game::Game(Flags flags, Dat2u pos, Dat2hu size, unsigned short int frameRate, std::string title) : player({256, 256}), flags(flags), pos(pos), size(size), frameRate(frameRate), title(title) {};

Game::~Game() {
    if (backgroundTexture != nullptr) SDL_DestroyTexture(backgroundTexture);
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
    if (window != nullptr) SDL_DestroyWindow(window);

    if (renderer != nullptr) SDL_DestroyRenderer(renderer);

    // quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
};

/**
 * @brief The only method accessible at public scope.
 * @note Call this exactly once.
*/
void Game::start() {
    init();
    gameLoop();
}

/**
 * @brief Initialize SDL subsystems & member classes.
 * @note Any `init()` methods should be placed here.
*/
void Game::init() {
    if (SDL_Init(flags.init) < 0) std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
    for (const auto& pair: flags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    if (!(IMG_Init(flags.image) & flags.image)) std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;   // https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/index2.php

    window = SDL_CreateWindow(title.c_str(), pos._x, pos._y, size._x, size._y, flags.window);
    windowID = SDL_GetWindowID(window);

    renderer = SDL_CreateRenderer(window, -1, flags.renderer);
    SDL_SetRenderDrawColor(renderer, 0x69, 0x69, 0x69, SDL_ALPHA_OPAQUE);   // throw some arbitrary color hexvalue

    player.init(window, renderer);

    changeWindowSurface();
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::gameLoop() {
    // int last_ticks = SDL_GetTicks64();

    while (state != GameState::EXIT) {
        // if (SDL_GetTicks64() - last_ticks < 1000 / frameRate) continue;
        // last_ticks = SDL_GetTicks64();
        handleEvents();
        render();
    }
}

/**
 * @brief GPU-render textures.
 * @note Must be called every loop, even if nothing updates.
 * @note Any `render()` methods should be placed here.
*/
void Game::render() {
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    player.move();
    if (state == GameState::PLAYING) player.render(renderer);

    SDL_RenderPresent(renderer);
}

/**
 * @brief Handle all incoming events.
 * @note All `handleEvent()` methods should go here.
*/
void Game::handleEvents() {
    SDL_Event* event = new SDL_Event;
    if (!SDL_PollEvent(event)) return;

    switch (event -> type) {
        case SDL_QUIT:
            state = GameState::EXIT;
            std::cout << "exit signal received, program terminated successfully." << std::endl;
            break;
        
        case SDL_WINDOWEVENT:
            handleWindowEvent(event);
            break;
        // track mouse motion & buttons only
        // also invoked when mouse focus regained/lost
        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
            break;
        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(event);
            break;
    }

    delete event;
}

void Game::handleWindowEvent(const SDL_Event* event) {
    if (event -> window.windowID != windowID) return;
    switch (event -> window.event) {
        case SDL_WINDOWEVENT_MOVED:
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            changeWindowSurface();
            // std::cout << "window-size-changed" << std::endl;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            // std::cout << "window-minimized" << std::endl;
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            // std::cout << "window-maximized" << std::endl;
            break;
        case SDL_WINDOWEVENT_ENTER:
            // std::cout << "window-focus-mouse-gained" << std::endl;
            break;
        case SDL_WINDOWEVENT_LEAVE:
            // std::cout << "window-focus-mouse-lost" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            // std::cout << "window-focus-keyboard-gained" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            // std::cout << "window-focus-keyboard-lost" << std::endl;
            break;
    }
}

/**
 * @note Scancode denotes physical location and keycode denotes actual meaning (different if remapped)
*/
void Game::handleKeyBoardEvent(const SDL_Event* event) {
    if (event -> key.keysym.sym == SDLK_ESCAPE) {
        state = GameState::EXIT;
        return;
    }
    // std::cout << "key-pressed: [" << SDL_GetKeyName(event -> key.keysym.sym) << "]" << std::endl;
    std::string nextTitle;

    switch (state) {
        case GameState::PLAYING:
            // control player movements
            player.handleKeyboardEvent(event);

            nextTitle = title + " (" + std::to_string(player.destRect.x) + ", " + std::to_string(player.destRect.y) + ")";
            SDL_SetWindowTitle(window, nextTitle.c_str());

            if (event -> key.keysym.sym == SDLK_ESCAPE) state = GameState::EXIT;
    }
}

void Game::handleMouseEvent(const SDL_Event* event) {
    mouse.state = SDL_GetMouseState(&mouse.pos._x, &mouse.pos._y);

    switch (state) {
        case GameState::MENU:
            if (event -> type != SDL_MOUSEBUTTONDOWN) break;
            state = GameState::PLAYING;
            changeBackground();
            break;

        case GameState::PLAYING:
            player.handleMouseEvent(event);
    }
}

/**
 * @brief Change the surface to the window.
 * @note Should also be called when an `SDL_WINDOWEVENT_SIZE_CHANGED` event occurs.
 * @see Game::handleWindowEvent()
*/
void Game::changeWindowSurface() {
    windowSurface = SDL_GetWindowSurface(window);
    changeBackground();
}

/**
 * @brief Change the window's background.
 * @note Should at least be called whenever `state` updates.
*/
void Game::changeBackground() {
    std::string path;

    switch (state) {
        case GameState::MENU:
            path = "assets/images/backgrounds/menu.png";
        case GameState::EXIT: break;
        default:
            // generate map tiles
            path = "assets/images/backgrounds/yuubee.png";
    }

    backgroundTexture = IMG_LoadTexture(renderer, path.c_str());
}