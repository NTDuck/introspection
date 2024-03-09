#include <game.hpp>

#include <algorithm>
#include <string>
#include <limits>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <mixer.hpp>
#include <timers.hpp>
#include <interface.hpp>
#include <auxiliaries.hpp>


Game::Game(GameInitFlag const& flags, SDL_Rect windowDimension, const int FPS, const std::string title) : mFlags(flags), mWindowDimension(windowDimension), mFPS(FPS), mWindowTitle(title) {}

Game::~Game() {
    if (mWindowSurface != nullptr) {
        SDL_FreeSurface(mWindowSurface);
        mWindowSurface = nullptr;
    }
    if (mWindowIcon != nullptr) {
        SDL_FreeSurface(mWindowIcon);
        mWindowIcon = nullptr;
    }
    if (mWindow != nullptr) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    globals::deinitialize();

    FPSDisplayTimer::deinitialize();
    FPSControlTimer::deinitialize();
    FPSOverlay::deinitialize();
    ExitText::deinitialize();

    Mixer::deinitialize();

    IngameInterface::deinitialize();
    MenuInterface::deinitialize();
    LoadingInterface::deinitialize();

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();
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
    SDL_Init(mFlags.lSDL);
    IMG_Init(mFlags.lIMG);
    TTF_Init();
    Mix_OpenAudio(config::mixer::frequency, config::mixer::format, config::mixer::channels, config::mixer::chunkSize);

    for (const auto& pair: mFlags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    mWindow = SDL_CreateWindow(mWindowTitle.c_str(), mWindowDimension.x, mWindowDimension.y, mWindowDimension.w, mWindowDimension.h, mFlags.window);
    mWindowIcon = IMG_Load(mWindowIconPath.generic_string().c_str());
    SDL_SetWindowIcon(mWindow, mWindowIcon);
    mWindowID = SDL_GetWindowID(mWindow);
    globals::renderer = SDL_CreateRenderer(mWindow, -1, mFlags.renderer);

    event::initialize();
    
    // Initialize dependencies
    IngameInterface::initialize();
    MenuInterface::initialize();
    LoadingInterface::initialize();
    GameOverInterface::initialize();

    FPSDisplayTimer::instantiate();
    FPSControlTimer::instantiate();
    FPSOverlay::instantiate(config::components::fps_overlay::initializer);
    ExitText::instantiate(config::components::exit_text::initializer);

    Mixer::instantiate();

    IngameInterface::instantiate();
    MenuInterface::instantiate();   // Requires instantiation of `Player` and `IngameMapHandler`
    LoadingInterface::instantiate();
    GameOverInterface::instantiate();
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::startGameLoop() {
    // Serve as partial initialization for certain attributes of certain classes
    // onLevelChange();
    onWindowChange();

    FPSDisplayTimer::invoke(&FPSDisplayTimer::start);

    while (globals::state != GameState::kExit) {
        // Control frame rate
        FPSControlTimer::invoke(&FPSControlTimer::start);

        // Calculate frame rate
        FPSDisplayTimer::invoke(&FPSDisplayTimer::calculateFPS);
        if (FPSDisplayTimer::instance->mAccumulatedFrames % FPSOverlay::kAnimationUpdateRate == 0) FPSOverlay::invoke(&FPSOverlay::editContent, config::components::fps_overlay::prefix + utils::castDoubleToString(FPSDisplayTimer::instance->mAverageFPS, config::components::fps_overlay::precision));

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
        case GameState::kIngameDialogue:
            IngameInterface::invoke(&IngameInterface::render);
            break;

        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::render);
            break;

        case GameState::kLoading:
            LoadingInterface::invoke(&LoadingInterface::render);
            break;

        case GameState::kGameOver:
            GameOverInterface::invoke(&GameOverInterface::render);
            break;

        default: break;
    }

    // Dependencies that should render regardless of game state
    FPSOverlay::invoke(&FPSOverlay::render);
    ExitText::invoke(&ExitText::render);

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
    mWindowSurface = SDL_GetWindowSurface(mWindow);
    SDL_GetWindowSize(mWindow, &globals::windowSize.x, &globals::windowSize.y);

    FPSOverlay::invoke(&FPSOverlay::onWindowChange);
    ExitText::invoke(&ExitText::onWindowChange);

    // Dependencies that rely on certain dimension-related global variables
    IngameInterface::invoke(&IngameInterface::onWindowChange);
    MenuInterface::invoke(&MenuInterface::onWindowChange);
    LoadingInterface::invoke(&LoadingInterface::onWindowChange);
    GameOverInterface::invoke(&GameOverInterface::onWindowChange);

    SDL_UpdateWindowSurface(mWindow);
}

/**
 * @brief Handle everything about entities.
 * @note Some `Mixer::invoke(&Mixer::handleGameStateChange)` has been commented out to prevent overlap with `onLevelChange()`.
*/
void Game::handleDependencies() {
    Mixer::invoke(&Mixer::handleGameStateChange);

    switch (globals::state) {
        case GameState::kIngamePlaying:
        case GameState::kIngameDialogue:
            IngameInterface::invoke(&IngameInterface::handleDependencies);
            break;

        case GameState::kMenu:
            Mixer::invoke(&Mixer::handleGameStateChange);
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

        case GameState::kGameOver:
            Mixer::invoke(&Mixer::handleGameStateChange);
            break;

        default: break;
    }
}

/**
 * @brief Handle all incoming events.
 * @note All `handleEvent()` methods should go here.
*/
void Game::handleEvents() {
    handleCustomEventPOST();

    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Mixer::invoke(&Mixer::stopBGM);
                globals::state = GameState::kExit;
                break;
            
            case SDL_WINDOWEVENT:
                handleWindowEvent(event);
                break;

            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:
                handleMouseEvent(event);
                break;

            case SDL_KEYDOWN: case SDL_KEYUP:
                handleKeyBoardEvent(event);
                break;

            default:
                if (event.type != event::type) break;
                handleCustomEventGET(event);
        }
    }
}

/**
 * @brief Handle a windows event.
 * @bug Event `SDL_WINDOWEVENT_SIZE_CHANGED` causes temporary undefined behavior for non-stationary entities.
*/
void Game::handleWindowEvent(SDL_Event const& event) {
    if (event.window.windowID != mWindowID) return;
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
void Game::handleKeyBoardEvent(SDL_Event const& event) const {
    ExitText::invoke(&ExitText::handleKeyBoardEvent, event);
    if (event.key.repeat != 0) return;

    switch (globals::state) {
        case GameState::kIngamePlaying:
        case GameState::kIngameDialogue:
            IngameInterface::invoke(&IngameInterface::handleKeyBoardEvent, event);
            break;

        default: break;
    }
}

/**
 * @brief Handle a mouse event.
*/
void Game::handleMouseEvent(SDL_Event const& event) const {
    SDL_GetMouseState(&globals::mouseState.x, &globals::mouseState.y);

    switch (globals::state) {
        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::handleMouseEvent, event);
            break;

        case GameState::kIngamePlaying:
        case GameState::kIngameDialogue:
            IngameInterface::invoke(&IngameInterface::handleMouseEvent, event);
            break;

        case GameState::kGameOver:
            GameOverInterface::invoke(&GameOverInterface::handleMouseEvent, event);
            break;

        default: break;
    }
}

void Game::handleCustomEventGET(SDL_Event const& event) const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::handleCustomEventGET, event);
            break;

        default: break;
    }

    event::terminate(event);
}

void Game::handleCustomEventPOST() const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::handleCustomEventPOST);
            break;

        default: break;
    }
}