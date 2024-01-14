#include <game.hpp>

#include <algorithm>
#include <string>
#include <limits>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <timers.hpp>
#include <interface.hpp>
#include <auxiliaries.hpp>


Game::Game(GameInitFlag const& flags, SDL_Rect windowDimension, const int frameRate, const std::string title) : flags(flags), windowDimension(windowDimension), frameRate(frameRate), title(title) {}

Game::~Game() {
    if (windowSurface != nullptr) {
        SDL_FreeSurface(windowSurface);
        windowSurface = nullptr;
    }
    
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    globals::deinitialize();
    FPSDisplayTimer::deinitialize();
    FPSControlTimer::deinitialize();
    FPSOverlay::deinitialize();

    IngameInterface::deinitialize();
    MenuInterface::deinitialize();
    LoadingInterface::deinitialize();

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

/**
 * @brief The only non-static method accessible at public scope.
 * @note Call this exactly once.
*/
void Game::start() {
    initialize();
    startGameLoop();
}

/**
 * @brief Initialize everything.
 * @note Any `initialize()` methods should be placed here.
*/
void Game::initialize() {
    // Initialize SDL subsystems
    SDL_Init(flags.lSDL);
    IMG_Init(flags.lIMG);
    TTF_Init();

    for (const auto& pair: flags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    window = SDL_CreateWindow(title.c_str(), windowDimension.x, windowDimension.y, windowDimension.w, windowDimension.h, flags.window);
    windowID = SDL_GetWindowID(window);
    globals::renderer = SDL_CreateRenderer(window, -1, flags.renderer);
    
    // IngameMapHandler::initialize();
    IngameInterface::initialize();
    MenuInterface::initialize();
    LoadingInterface::initialize();

    FPSDisplayTimer::instantiate();
    FPSControlTimer::instantiate();
    FPSOverlay::instantiate(config::components::fps_overlay::initializer);

    IngameInterface::instantiate();
    MenuInterface::instantiate();   // Requires instantiation of `Player` and `IngameMapHandler`
    LoadingInterface::instantiate();
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::startGameLoop() {
    // Serve as partial initialization for certain attributes of certain classes
    onLevelChange();
    onWindowChange();

    FPSDisplayTimer::invoke(&FPSDisplayTimer::start);

    while (globals::state != GameState::kExit) {
        // Control frame rate
        FPSControlTimer::invoke(&FPSControlTimer::start);

        // Calculate frame rate
        FPSDisplayTimer::invoke(&FPSDisplayTimer::calculateFPS);
        if (FPSDisplayTimer::instance->accumulatedFrames % FPSOverlay::animationUpdateRate == 0) FPSOverlay::invoke(&FPSOverlay::editContent, std::to_string(FPSDisplayTimer::instance->averageFPS));

        // Main flow
        handleDependencies();
        handleEvents();
        render();

        // Control frame rate
        FPSControlTimer::invoke(&FPSControlTimer::controlFPS);
    }
}

/**
 * @brief Handle everything about rendering.
 * 
 * @note The order should be as follows: `interface` i.e. environments -> interactables -> entities -> player
 * @note Any `render()` methods should be placed here.
*/
void Game::render() const {
    SDL_RenderClear(globals::renderer);

    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::render);
            break;

        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::render);
            break;

        case GameState::kLoading:
            LoadingInterface::invoke(&LoadingInterface::render);
            break;

        default: break;
    }

    // Dependencies that should render regardless of game state
    FPSOverlay::invoke(&FPSOverlay::render);

    SDL_RenderPresent(globals::renderer);
}

/**
 * @brief Called when switching to a new level.
*/
void Game::onLevelChange() {
    IngameInterface::invoke(&IngameInterface::onLevelChange);
}

/**
 * @brief Called should `window` change e.g. is resized.
*/
void Game::onWindowChange() {
    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &globals::windowSize.x, &globals::windowSize.y);

    FPSOverlay::invoke(&FPSOverlay::onWindowChange);

    // Dependencies that rely on certain dimension-related global variables
    IngameInterface::invoke(&IngameInterface::onWindowChange);
    MenuInterface::invoke(&MenuInterface::onWindowChange);
    LoadingInterface::invoke(&LoadingInterface::onWindowChange);

    SDL_UpdateWindowSurface(window);
}

/**
 * @brief Handle everything about entities.
*/
void Game::handleDependencies() {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::handleEntities);
            break;

        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::updateAnimation);
            break;

        case (GameState::kLoading | GameState::kIngamePlaying):
            LoadingInterface::invoke(&LoadingInterface::initiateTransition, GameState::kIngamePlaying);
            onLevelChange();
            onWindowChange();
            break;

        case (GameState::kLoading | GameState::kMenu):
            LoadingInterface::invoke(&LoadingInterface::initiateTransition, GameState::kMenu);
            break;

        case GameState::kLoading:
            LoadingInterface::invoke(&LoadingInterface::updateAnimation);
            LoadingInterface::invoke(&LoadingInterface::handleTransition);
            break;

        default: break;
    }
}

/**
 * @brief Handle all incoming events.
 * @note All `handleEvent()` methods should go here.
*/
void Game::handleEvents() {
    SDL_Event* event = new SDL_Event;
    if (!SDL_PollEvent(event)) return;

    switch (event->type) {
        case SDL_QUIT:
            globals::state = GameState::kExit;
            break;
        
        case SDL_WINDOWEVENT:
            handleWindowEvent(*event);
            break;

        // track mouse motion & buttons only
        // also invoked when mouse focus regained/lost
        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
            handleMouseEvent(*event);
            break;

        case SDL_KEYDOWN: case SDL_KEYUP:
            handleKeyBoardEvent(*event);
            break;
    }

    delete event;
}

/**
 * @brief Handle a windows event.
 * @bug Event `SDL_WINDOWEVENT_SIZE_CHANGED` causes temporary undefined behavior for non-stationary entities.
*/
void Game::handleWindowEvent(const SDL_Event& event) {
    if (event.window.windowID != windowID) return;
    switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            onWindowChange();
            break;
        default: break;
    }
}

/**
 * @brief Handle a keyboard event.
 * @note Scancode denotes physical location and keycode denotes actual meaning (different if remapped)
*/
void Game::handleKeyBoardEvent(const SDL_Event& event) const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::handleKeyBoardEvent, event);
            break;

        default: break;
    }
}

/**
 * @brief Handle a mouse event.
*/
void Game::handleMouseEvent(const SDL_Event& event) {
    SDL_GetMouseState(&globals::mouseState.x, &globals::mouseState.y);

    switch (globals::state) {
        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::handleMouseEvent, event);
            break;

        default: break;
    }
}