#include <game.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include <interaction.hpp>
#include <interface.hpp>
#include <entities.hpp>
#include <auxiliaries/globals.hpp>


Game::Game(const GameFlag& flags, SDL_Rect windowDimension, const int frameRate, const std::string title) : window(nullptr), windowSurface(nullptr), interface(nullptr), player(nullptr), flags(flags), windowDimension(windowDimension), frameRate(frameRate), title(title) {}

Game::~Game() {
    if (windowSurface != nullptr) SDL_FreeSurface(windowSurface);
    if (window != nullptr) SDL_DestroyWindow(window);

    globals::deinitialize();
    IngameInterface::deinitialize();
    Player::deinitialize();
    Teleporter::deinitialize();
    Slime::deinitialize();

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
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
    // Initialize SDL subsystems.
    SDL_Init(flags.init);
    IMG_Init(flags.image);
    for (const auto& pair: flags.hints) SDL_SetHint(pair.first.c_str(), pair.second.c_str());

    window = SDL_CreateWindow(title.c_str(), windowDimension.x, windowDimension.y, windowDimension.w, windowDimension.h, flags.window);
    windowID = SDL_GetWindowID(window);

    globals::renderer = SDL_CreateRenderer(window, -1, flags.renderer);

    state = GameState::kMenu;

    IngameInterface::initialize();
    Player::initialize();
    Teleporter::initialize();
    Slime::initialize();

    interface = IngameInterface::instantiate(globals::config::kDefaultLevelName);
    player = Player::instantiate();
}

/**
 * @brief Start the game loop.
 * @todo Manually cap frame rate (VSync disabled)
*/
void Game::startGameLoop() {
    // Serve as partial initialization for certain attributes of certain classes
    onLevelChange();
    onWindowChange();

    while (state != GameState::kExit) {
        handleEntities();
        handleEvents();
        render();
    }
}

/**
 * @brief Handle everything about rendering.
 * 
 * @note The order should be as follows: `interface` i.e. environments -> interactables -> entities -> player
 * @note Any `render()` methods should be placed here.
*/
void Game::render() {
    SDL_RenderClear(globals::renderer);

    if (state == GameState::kIngamePlaying) {
        interface->render();
        Teleporter::callOnEach(&Teleporter::render);
        Slime::callOnEach(&Slime::render);
        player->render();
    }

    SDL_RenderPresent(globals::renderer);
}

/**
 * @brief Called when switching to a new level.
*/
void Game::onLevelChange() {
    // Populate `globals::currentLevelData` members
    interface->onLevelChange();

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    player->onLevelChange(globals::currentLevelData.playerLevelData);
    Teleporter::callOnEach_onLevelChange<level::TeleporterLevelData>(globals::currentLevelData.teleportersLevelData);
    Slime::callOnEach_onLevelChange<level::SlimeLevelData>(globals::currentLevelData.slimesLevelData);
}

/**
 * @brief Called should `window` change e.g. is resized.
*/
void Game::onWindowChange() {
    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &globals::windowSize.x, &globals::windowSize.y);

    // Dependencies that rely on certain dimension-related global variables
    interface->onWindowChange();
    player->onWindowChange();
    Teleporter::callOnEach(&Teleporter::onWindowChange);
    Slime::callOnEach(&Slime::onWindowChange);

    SDL_UpdateWindowSurface(window);
}

/**
 * @brief Handle everything about entities.
*/
void Game::handleEntities() {
    if (state == GameState::kIngamePlaying) {
        handleEntitiesMovement();
        handleEntitiesInteraction();
    }
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void Game::handleEntitiesMovement() {
    player->initiateAnimation();
    player->move();
    player->updateAnimation();

    Teleporter::callOnEach(&Teleporter::updateAnimation);

    Slime::callOnEach(&Slime::calculateMove, player->destCoords);
    Slime::callOnEach(&Slime::initiateAnimation);
    Slime::callOnEach(&Slime::move);
    Slime::callOnEach(&Slime::updateAnimation);
}

template <class Active, class Passive>
void Game::onEntityCollision(Active& active, Passive& passive) {}

template <>
void Game::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter) {
    state = GameState::kIngamePaused;
    interface->changeLevel(teleporter.targetLevel);
    globals::currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;
    onLevelChange(); onWindowChange();
    state = GameState::kIngamePlaying;
}

template <>
void Game::onEntityCollision<Player, Slime>(Player& player, Slime& slime) {
    // state = GameState::kExit;
    // player.onDeath();
}

/**
 * @brief Called when the `active` entity initiate an animation (possibly caused by the `passive` entity).
*/
template <class Active, class Passive>
void Game::onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) {
    // Handle `kDamaged` case differently
    if (animationType == AnimationType::kDamaged && passive.currAnimationType == AnimationType::kAttack) {
        active.secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(passive.secondaryStats, active.secondaryStats);
        if (active.secondaryStats.HP <= 0) animationType = AnimationType::kDeath;
    }
    tile::NextAnimationData::update(active.nextAnimationData, animationType);
}

template void Game::onEntityAnimation<Player, Slime>(const AnimationType animationType, Player& player, Slime& slime);
template void Game::onEntityAnimation<Slime, Player>(const AnimationType animationType, Slime& slime, Player& player);

/**
 * @brief Handle interactions between entities.
*/
void Game::handleEntitiesInteraction() {
    auto teleporter = utils::checkEntityCollision<Player, Teleporter>(*player, InteractionType::kCoords); if (teleporter != nullptr) onEntityCollision<Player, Teleporter>(*player, *teleporter);
    auto slime = utils::checkEntityCollision<Player, Slime>(*player, InteractionType::kRect); if (slime != nullptr) onEntityCollision<Player, Slime>(*player, *slime);

    for (auto& slime : Slime::instances) {
        if (slime == nullptr || slime->currAnimationType == AnimationType::kDeath) continue;
        if (utils::checkEntityAttackInitiate<Slime, Player>(*slime, *player)) onEntityAnimation<Slime, Player>(AnimationType::kAttack, *slime, *player);
        if (utils::checkEntityAttackRegister<Player, Slime>(*player, *slime)) onEntityAnimation<Player, Slime>(AnimationType::kDamaged, *player, *slime);
        if (utils::checkEntityAttackRegister<Slime, Player>(*slime, *player)) onEntityAnimation<Slime, Player>(AnimationType::kDamaged, *slime, *player);
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
            state = GameState::kExit;
            delete instance;   // ?
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
void Game::handleKeyBoardEvent(const SDL_Event& event) {
    switch (state) {
        case GameState::kIngamePlaying:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                state = GameState::kExit;
                break;
            }
            player->handleKeyboardEvent(event);

        default: break;
    }
}

/**
 * @brief Handle a mouse event.
*/
void Game::handleMouseEvent(const SDL_Event& event) {
    switch (state) {
        case GameState::kMenu: case GameState::kIngamePlaying:
            if (event.type != SDL_MOUSEBUTTONDOWN) break;
            onLevelChange(); onWindowChange();
            state = GameState::kIngamePlaying;
            break;

        default: break;
    }
}


Game* Game::instance = nullptr;