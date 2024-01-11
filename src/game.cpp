#include <game.hpp>

#include <algorithm>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <interaction.hpp>
#include <interface.hpp>
#include <entities.hpp>
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

    IngameInterface::deinitialize();
    MenuInterface::deinitialize();
    LoadingInterface::deinitialize();

    Player::deinitialize();
    Teleporter::deinitialize();
    Slime::deinitialize();

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

    IngameInterface::initialize();
    MenuInterface::initialize();
    LoadingInterface::initialize();

    Player::initialize();
    Teleporter::initialize();
    Slime::initialize();

    Player::instantiate(SDL_Point{ 0, 0 });
    IngameInterface::instantiate(config::interface::levelName);
    MenuInterface::instantiate();   // Requires instantiation of `Player` and `IngameInterface`
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

    while (globals::state != GameState::kExit) {
        handleDependencies();
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
void Game::render() const {
    SDL_RenderClear(globals::renderer);

    switch (globals::state) {
        case GameState::kIngamePlaying:
            IngameInterface::invoke(&IngameInterface::render);
            Teleporter::invoke(&Teleporter::render);
            Slime::invoke(&Slime::render);
            Player::invoke(&Player::render);
            break;

        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::render);
            break;

        case GameState::kLoading:
            LoadingInterface::invoke(&LoadingInterface::render);
            break;

        default: break;
    }

    SDL_RenderPresent(globals::renderer);
}

/**
 * @brief Called when switching to a new level.
*/
void Game::onLevelChange() {
    // Populate `globals::currentLevelData` members
    IngameInterface::invoke(&IngameInterface::onLevelChange);

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, globals::currentLevelData.playerLevelData);
    Teleporter::onLevelChangeAll<level::TeleporterLevelData>(globals::currentLevelData.teleportersLevelData);
    Slime::onLevelChangeAll<level::SlimeLevelData>(globals::currentLevelData.slimesLevelData);
}

/**
 * @brief Called should `window` change e.g. is resized.
*/
void Game::onWindowChange() {
    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &globals::windowSize.x, &globals::windowSize.y);

    // Dependencies that rely on certain dimension-related global variables
    IngameInterface::invoke(&IngameInterface::onWindowChange);
    MenuInterface::invoke(&MenuInterface::onWindowChange);
    LoadingInterface::invoke(&LoadingInterface::onWindowChange);

    Player::invoke(&Player::onWindowChange);
    Teleporter::invoke(&Teleporter::onWindowChange);
    Slime::invoke(&Slime::onWindowChange);

    SDL_UpdateWindowSurface(window);
}

/**
 * @brief Handle everything about entities.
*/
void Game::handleDependencies() {
    handleInterfaces();
    handleEntities();
}

void Game::handleInterfaces() {
    switch (globals::state) {
        case GameState::kMenu:
            MenuInterface::invoke(&MenuInterface::updateAnimation);
            break;

        case (GameState::kLoading | GameState::kIngamePlaying):
            LoadingInterface::invoke(&LoadingInterface::initiateTransition, GameState::kIngamePlaying);
            onLevelChange();
            onWindowChange();
            break;

        case GameState::kLoading:
            LoadingInterface::invoke(&LoadingInterface::updateAnimation);
            LoadingInterface::invoke(&LoadingInterface::handleTransition);
            break;

        default: break;
    }
}

void Game::handleEntities() {
    if (globals::state != GameState::kIngamePlaying) return;
    handleEntitiesMovement();
    handleEntitiesInteraction();
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void Game::handleEntitiesMovement() {
    Player::invoke(&Player::initiateAnimation);
    Player::invoke(&Player::move);
    Player::invoke(&Player::updateAnimation);

    Teleporter::invoke(&Teleporter::updateAnimation);

    Slime::invoke(&Slime::calculateMove, Player::instance->destCoords);
    Slime::invoke(&Slime::initiateAnimation);
    Slime::invoke(&Slime::move);
    Slime::invoke(&Slime::updateAnimation);
}

template <typename Active, typename Passive>
void Game::onEntityCollision(Active& active, Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedDynamicEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();
}

template <>
void Game::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter) {
    IngameInterface::invoke(&IngameInterface::changeLevel, teleporter.targetLevel);
    globals::currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;

    globals::state = GameState::kLoading | GameState::kIngamePlaying;
}

template <>
void Game::onEntityCollision<Player, Slime>(Player& player, Slime& slime) {
    // state = GameState::kExit;
    // player.onDeath();
}

/**
 * @brief Called when the `active` entity initiate an animation (possibly caused by the `passive` entity).
*/
template <typename Active, typename Passive>
void Game::onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();

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
    auto teleporter = utils::checkEntityCollision<Player, Teleporter>(*Player::instance, InteractionType::kCoords); if (teleporter != nullptr) onEntityCollision<Player, Teleporter>(*Player::instance, *teleporter);
    auto slime = utils::checkEntityCollision<Player, Slime>(*Player::instance, InteractionType::kRect); if (slime != nullptr) onEntityCollision<Player, Slime>(*Player::instance, *slime);

    for (auto& slime : Slime::instances) {
        if (slime == nullptr || slime->currAnimationType == AnimationType::kDeath) continue;
        if (utils::checkEntityAttackInitiate<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kAttack, *slime, *Player::instance);
        if (utils::checkEntityAttackRegister<Player, Slime>(*Player::instance, *slime)) onEntityAnimation<Player, Slime>(AnimationType::kDamaged, *Player::instance, *slime);
        if (utils::checkEntityAttackRegister<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kDamaged, *slime, *Player::instance);
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
            // delete instance;   // ?
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
    switch (globals::state) {
        case GameState::kIngamePlaying:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                globals::state = GameState::kMenu;
                break;
            }
            Player::invoke(&Player::handleKeyboardEvent, event);

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

        case GameState::kIngamePlaying:
            if (event.type != SDL_MOUSEBUTTONDOWN) break;
            onLevelChange(); onWindowChange();
            break;

        default: break;
    }
}