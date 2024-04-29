#include <interface.hpp>

#include <array>
#include <fstream>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    static constexpr auto renderIngameDependencies = []() {
        Invoker<IngameMapHandler, NON_INTERACTABLES, INTERACTABLES, TELEPORTERS, HOSTILES, SURGE_PROJECTILES, Player>::invoke_render();
    };

    IngameDialogueBox::instantiate(config::components::dialogue_box::initializer);

    Player::instantiate(SDL_Point{});   // This is required for below instantiations
    IngameMapHandler::instantiate(config::interface::levelName);
    IngameViewHandler::instantiate(renderIngameDependencies, Player::instance->mDestRect);
}

IngameInterface::~IngameInterface() {
    if (mCachedTargetDestCoords != nullptr) delete mCachedTargetDestCoords;
}

/**
 * @note Intellisense's `more than one instance of overloaded function "[...]::deinitialize" matches the argument list:C/C++(308)` could be safely ignored as no relevant warnings are shown during compilation.
*/
void IngameInterface::deinitialize() {
    Invoker<IngameMapHandler, IngameViewHandler, Player, NON_INTERACTABLES, PlaceholderInteractable, INTERACTABLES, PlaceholderTeleporter, TELEPORTERS, HOSTILES, SURGE_PROJECTILES, IngameDialogueBox>::invoke_deinitialize();
}

void IngameInterface::initialize() {
    Invoker<IngameMapHandler, Player, NON_INTERACTABLES, PlaceholderInteractable, INTERACTABLES, PlaceholderTeleporter, TELEPORTERS, HOSTILES, SURGE_PROJECTILES>::invoke_initialize();
}

void IngameInterface::render() const {
    IngameViewHandler::invoke(&IngameViewHandler::render);

    switch (globals::state) {
        case GameState::kIngameDialogue:
            IngameDialogueBox::invoke(&IngameDialogueBox::render);
            break;

        default: break;
    }
}

void IngameInterface::onLevelChange() const {
    auto levelName = IngameMapHandler::instance->getLevel();
    
    // Populate `globals::currentLevelData` members
    IngameMapHandler::invoke(&IngameMapHandler::onLevelChange);
    IngameViewHandler::invoke(&IngameViewHandler::onLevelChange);

    // Populate `level::data.properties` members
    switch (levelName) {
        case level::Name::kLevelWhiteSpace:
            level::data.setProperty<bool>("is-border-traversed", false);
            break;

        default: break;
    }
    
    if (mCachedTargetDestCoords != nullptr && levelName != config::interface::levelName) {
        auto data = new level::Data_Generic();
        data->destCoords = *mCachedTargetDestCoords;
        level::data.erase(config::entities::player::typeID);
        level::data.insert(config::entities::player::typeID, data);
    }

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    auto playerLevelData = level::data.get(config::entities::player::typeID);
    if (!playerLevelData.empty() && playerLevelData.front() != nullptr) Player::invoke(&Player::onLevelChange, *playerLevelData.front());

    Invoker<NON_INTERACTABLES, PlaceholderInteractable, INTERACTABLES, PlaceholderTeleporter, TELEPORTERS, HOSTILES, SURGE_PROJECTILES>::invoke_onLevelChangeAll();
    Mixer::invoke(&Mixer::onLevelChange, levelName);   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    Invoker<IngameMapHandler, IngameViewHandler, Player, NON_INTERACTABLES, INTERACTABLES, TELEPORTERS, HOSTILES, SURGE_PROJECTILES, IngameDialogueBox>::invoke_onWindowChange();
}

void IngameInterface::handleKeyBoardEvent(SDL_Event const& event) const {
    switch (event.key.keysym.sym) {
        case ~config::Key::kIngameReturnMenu:
            if (event.type != SDL_KEYDOWN) break;

            // Save progress
            if (Player::instance->mAnimation != Animation::kDeath) {
                auto data = saveProgressToJSON();
                saveJSONToStorage(data);
            }

            globals::state = GameState::kLoading | GameState::kMenu;
            break;

        case ~config::Key::kIngameLevelReset:
            if (event.type != SDL_KEYDOWN) break;
            onLevelChange(); onWindowChange();
            break;

        case ~config::Key::kIngameDialogueTest:
            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, config::components::dialogue_box::test);
            break;

        default: break;
    }

    IngameViewHandler::invoke(&IngameViewHandler::handleKeyBoardEvent, event);
    IngameMapHandler::invoke(&IngameMapHandler::handleKeyBoardEvent, event);

    switch (globals::state) {
        case GameState::kIngamePlaying:
            Player::invoke(&Player::handleKeyboardEvent, event);
            break;

        case GameState::kIngameDialogue:
            IngameDialogueBox::invoke(&IngameDialogueBox::handleKeyBoardEvent, event);
            break;

        default: break;   // Unnecessary?
    }
}

void IngameInterface::handleMouseEvent(SDL_Event const& event) const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            Player::invoke(&Player::handleMouseEvent, event);
            break;

        default: break;
    }
}

/**
 * @note `GameState::kIngamePlaying` only.
*/
void IngameInterface::handleCustomEventGET(SDL_Event const& event) const {
    switch (static_cast<event::Code>(event.user.code)) {
        case event::Code::kResp_Teleport_GTE_Player:
            handleCustomEventGET_impl<event::Code::kResp_Teleport_GTE_Player>(event);
            break;

        case event::Code::kReq_DeathPending_Player:
            handleCustomEventGET_impl<event::Code::kReq_DeathPending_Player>();
            break;

        case event::Code::kReq_DeathFinalized_Player:
            handleCustomEventGET_impl<event::Code::kReq_DeathFinalized_Player>();
            Player::instance->mSecondaryStats.initialize(Player::instance->mPrimaryStats);   // Prevent stuck at death screen
            break;

        default: break;
    }

    Invoker<Player, PlaceholderInteractable, INTERACTABLES, PlaceholderTeleporter, TELEPORTERS, HOSTILES, SURGE_PROJECTILES>::invoke_handleCustomEventGET(event);
}

/**
 * @note `GameState::kIngamePlaying` only.
*/
void IngameInterface::handleCustomEventPOST() const {
    Invoker<Player, PlaceholderTeleporter, TELEPORTERS, HOSTILES, SURGE_PROJECTILES>::invoke_handleCustomEventPOST();
}

void IngameInterface::handleDependencies() const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            handleEntitiesInteraction();
            handleLevelSpecifics();
            handleEntitiesSFX();
            [[fallthrough]];

        case GameState::kIngameDialogue:
            IngameDialogueBox::invoke(&IngameDialogueBox::updateProgress);
            IngameDialogueBox::invoke(&IngameDialogueBox::handleSFX);
            break;

        default: break;
    }
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void IngameInterface::handleEntitiesInteraction() const {
    Invoker<NON_INTERACTABLES, INTERACTABLES, TELEPORTERS, HOSTILES, SURGE_PROJECTILES, Player>::invoke_updateAnimation();
    Invoker<SURGE_PROJECTILES>::invoke_handleInstantiation();
    Player::invoke(&Player::handleAutopilotMovement);   // Autopilot
    Invoker<HOSTILES, Player>::invoke_move();
}

void IngameInterface::handleLevelSpecifics() const {
    #define IMPL(ln) \
    case ln:\
        handleLevelSpecifics_impl<ln>();\
        break;

    switch (IngameMapHandler::instance->getLevel()) {     
        IMPL(level::Name::kLevelWhiteSpace)

        default: break;
    }
}

void IngameInterface::handleEntitiesSFX() const {
    Invoker<Player, HOSTILES>::invoke_handleSFX();
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kResp_Teleport_GTE_Player>
IngameInterface::handleCustomEventGET_impl(SDL_Event const& event) const {
    auto data = event::getData<event::Data_Teleporter>(event);
    
    IngameMapHandler::invoke(&IngameMapHandler::changeLevel, data.targetLevel);
    // globals::currentLevelData.playerLevelData.destCoords = data.targetDestCoords;   // This has no effect if placed here
    if (mCachedTargetDestCoords != nullptr) delete mCachedTargetDestCoords;
    mCachedTargetDestCoords = new SDL_Point(data.targetDestCoords);   // Cache

    // globals::state = GameState::kLoading | GameState::kIngamePlaying;
    onLevelChange();
    onWindowChange();
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_DeathPending_Player>
IngameInterface::handleCustomEventGET_impl() const {
    IngameMapHandler::instance->isOnGrayscale = true;
}

template <event::Code C>
typename std::enable_if_t<C == event::Code::kReq_DeathFinalized_Player>
IngameInterface::handleCustomEventGET_impl() const {
    globals::state = GameState::kGameOver;
    IngameMapHandler::instance->isOnGrayscale = false;
}

bool IngameInterface::isPlayerInRange(std::pair<int, int> const& x_lim, std::pair<int, int> const& y_lim) const {
    bool result = Player::instance->isWithinRange(x_lim, y_lim);

    if (result) {
        Player::invoke(&Player::onMoveEnd, BehaviouralType::kInvalidated);
        Player::invoke(&Player::onRunningToggled, false);
    }

    return result;
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelWhiteSpace>
IngameInterface::handleLevelSpecifics_impl() const {
    auto isBorderTraversed = level::data.getProperty<bool>("is-border-traversed");

    auto kInternalTeleportHandler = [&](int& i, const double lower, const double upper) {
        auto kInternalTeleportInitiate = [&]() {
            if (!isBorderTraversed) level::data.setProperty<bool>("is-border-traversed", true);
            RedHandThrone::invoke(&RedHandThrone::onWindowChange);
        };

        if (i <= lower) {
            i = upper;
            kInternalTeleportInitiate();
        } else if (i >= upper) {
            i = lower;
            kInternalTeleportInitiate();
        }

        // return (i <= lower) ? upper : (i >= upper) ? lower : i;
    };   // Declaring as `static` would yield `warning: storing the address of local variable ‘isBorderTraversed’ in ‘kInternalTeleportHandler.IngameInterface::handleLevelSpecifics_kLevelWhiteSpace() const::<lambda(int&, double, double)>::<isBorderTraversed capture>’ [-Wdangling-pointer=]`

    // "Infinite loop" effect
    if (Player::instance->mNextDestCoords != nullptr) {
        kInternalTeleportHandler(Player::instance->mNextDestCoords->x, IngameViewHandler::instance->mTileCountWidth / 2 + 1, level::data.tileDestCount.x - IngameViewHandler::instance->mTileCountWidth / 2 - 1);
        kInternalTeleportHandler(Player::instance->mNextDestCoords->y, IngameViewHandler::instance->mTileCountHeight / 2 + 2, level::data.tileDestCount.y - IngameViewHandler::instance->mTileCountHeight / 2 - 1);   // Slight deviation to prevent "staggering"
    }

    if (isBorderTraversed && RedHandThrone::instances.empty()) RedHandThrone::instantiateEX({
        new level::Data_Teleporter{ { 52, 43 }, { 20, 11 }, level::Name::kLevelInterlude },
    });
}

/**
 * @brief Save current in-game progress to external storage.
*/
json IngameInterface::saveProgressToJSON() const {
    // Populate JSON object
    json data;

    data["level"] = IngameMapHandler::instance->getLevel();
    data["player"]["x"] = Player::instance->mDestCoords.x;
    data["player"]["y"] = Player::instance->mDestCoords.y;

    return data;
}

void IngameInterface::saveJSONToStorage(json const& data) const {
    std::ofstream file;

    file.open(config::interface::savePath, std::ofstream::out);
    if (!file.is_open()) return;

    file << data.dump(2, ' ', true, json::error_handler_t::strict);   // Should move into `config::interface` ?
    file.close();
}

void IngameInterface::loadProgressFromStorage() const {
    if (!std::filesystem::exists(config::interface::savePath)) return;

    json data;
    utils::fetch(config::interface::savePath, data);
    if (data.empty()) return;

    IngameMapHandler::invoke(&IngameMapHandler::loadProgressFromStorage, data);

    // level::data.erase(config::entities::player::typeID);
    // level::data.insert(config::entities::player::typeID, new level::Data_Generic({ data["player"]["x"], data["player"]["y"] }));
    if (mCachedTargetDestCoords != nullptr) delete mCachedTargetDestCoords;
    mCachedTargetDestCoords = new SDL_Point({ data["player"]["x"], data["player"]["y"] });
}