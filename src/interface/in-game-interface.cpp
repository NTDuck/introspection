#include <interface.hpp>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    static constexpr auto renderIngameDependencies = []() {
        // Static assets
        IngameMapHandler::invoke(&IngameMapHandler::render);

        // Non-interactible entities
        OmoriLaptop::invoke(&OmoriLaptop::render);
        OmoriLightBulb::invoke(&OmoriLightBulb::render);
        OmoriMewO::invoke(&OmoriMewO::render);

        // Entities
        Teleporter::invoke(&Teleporter::render);
        RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::render);
        Slime::invoke(&Slime::render);

        // Projectiles
        PentacleProjectile::invoke(&PentacleProjectile::render);

        // Player must be rendered last
        Player::invoke(&Player::render);
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
    IngameMapHandler::deinitialize();
    IngameViewHandler::deinitialize();

    PentacleProjectile::deinitialize();

    Player::deinitialize();
    Teleporter::deinitialize();
    RedHandThroneTeleporter::deinitialize();
    Slime::deinitialize();

    OmoriLaptop::deinitialize();
    OmoriLightBulb::deinitialize();
    OmoriMewO::deinitialize();

    IngameDialogueBox::deinitialize();
}

void IngameInterface::initialize() {
    IngameMapHandler::initialize();

    PentacleProjectile::initialize();

    Player::initialize();
    Teleporter::initialize();
    RedHandThroneTeleporter::initialize();
    Slime::initialize();

    OmoriLaptop::initialize();
    OmoriLightBulb::initialize();
    OmoriMewO::initialize();
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
    // Populate `globals::currentLevelData` members
    IngameMapHandler::invoke(&IngameMapHandler::onLevelChange);
    IngameViewHandler::invoke(&IngameViewHandler::onLevelChange);
    
    if (mCachedTargetDestCoords != nullptr) {
        auto data = new level::Data_Generic();
        data->destCoords = *mCachedTargetDestCoords;
        level::data.insert(config::entities::player::typeID, data);
    }

    PentacleProjectile::onLevelChangeAll();

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, *(level::data.get(config::entities::player::typeID)[0]));
    Teleporter::onLevelChangeAll(level::data.get(config::entities::teleporter::typeID));
    RedHandThroneTeleporter::onLevelChangeAll(level::data.get(config::entities::teleporter_red_hand_throne::typeID));
    Slime::onLevelChangeAll(level::data.get(config::entities::slime::typeID));

    OmoriLaptop::onLevelChangeAll(level::data.get(config::entities::omori_laptop::typeID));
    OmoriLightBulb::onLevelChangeAll(level::data.get(config::entities::omori_light_bulb::typeID));
    OmoriMewO::onLevelChangeAll(level::data.get(config::entities::omori_mewo::typeID));

    Mixer::invoke(&Mixer::onLevelChange, IngameMapHandler::instance->getLevel());   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    IngameMapHandler::invoke(&IngameMapHandler::onWindowChange);
    IngameViewHandler::invoke(&IngameViewHandler::onWindowChange);

    PentacleProjectile::invoke(&PentacleProjectile::onWindowChange);

    Player::invoke(&Player::onWindowChange);
    Teleporter::invoke(&Teleporter::onWindowChange);
    RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::onWindowChange);
    Slime::invoke(&Slime::onWindowChange);

    OmoriLaptop::invoke(&OmoriLaptop::onWindowChange);
    OmoriLightBulb::invoke(&OmoriLightBulb::onWindowChange);
    OmoriMewO::invoke(&OmoriMewO::onWindowChange);

    IngameDialogueBox::invoke(&IngameDialogueBox::onWindowChange);
}

void IngameInterface::handleKeyBoardEvent(SDL_Event const& event) const {
    switch (event.key.keysym.sym) {
        case ~config::Key::kIngameReturnMenu:
            if (event.type != SDL_KEYDOWN) break;
            globals::state = GameState::kLoading | GameState::kMenu;
            break;

        case ~config::Key::kIngameLevelReset:
            if (event.type != SDL_KEYDOWN) break;
            onLevelChange(); onWindowChange();
            break;

        case ~config::Key::kIngameDialogueTest:
            IngameDialogueBox::invoke(&IngameDialogueBox::editContent, "Push pedestrians into oncoming traffic.");
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
            handleCustomEventGET_kResp_Teleport_GTE_Player(event);
            break;

        case event::Code::kReq_DeathPending_Player:
            handleCustomEventGET_kReq_DeathPending_Player();
            break;

        case event::Code::kReq_DeathFinalized_Player:
            handleCustomEventGET_kReq_DeathFinalized_Player();
            break;

        default: break;
    }

    PentacleProjectile::invoke(&PentacleProjectile::handleCustomEventGET, event);

    Player::invoke(&Player::handleCustomEventGET, event);
    Teleporter::invoke(&Teleporter::handleCustomEventGET, event);
    RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::handleCustomEventGET, event);
    Slime::invoke(&Slime::handleCustomEventGET, event);
}

/**
 * @note `GameState::kIngamePlaying` only.
*/
void IngameInterface::handleCustomEventPOST() const {
    PentacleProjectile::invoke(&PentacleProjectile::handleCustomEventPOST);

    Player::invoke(&Player::handleCustomEventPOST);
    Teleporter::invoke(&Teleporter::handleCustomEventPOST);
    RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::handleCustomEventPOST);
    Slime::invoke(&Slime::handleCustomEventPOST);
}

void IngameInterface::handleDependencies() const {
    switch (globals::state) {
        case GameState::kIngamePlaying:
            handleEntitiesInteraction();
            handleLevelSpecifics();
            handleEntitiesSFX();
            break;

        case GameState::kIngameDialogue:
            IngameDialogueBox::invoke(&IngameDialogueBox::updateContent);
            break;

        default: break;
    }
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void IngameInterface::handleEntitiesInteraction() const {
    PentacleProjectile::invoke(&PentacleProjectile::handleInstantiation);
    PentacleProjectile::handleTermination();
    PentacleProjectile::invoke(&PentacleProjectile::updateAnimation);

    Player::invoke(&Player::initiateAnimation);
    Player::invoke(&Player::move);
    Player::invoke(&Player::updateAnimation);

    Teleporter::invoke(&Teleporter::updateAnimation);
    RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::updateAnimation);

    Slime::invoke(&Slime::initiateAnimation);
    Slime::invoke(&Slime::move);
    Slime::invoke(&Slime::updateAnimation);

    OmoriLaptop::invoke(&OmoriLaptop::updateAnimation);
    OmoriLightBulb::invoke(&OmoriLightBulb::updateAnimation);
    OmoriMewO::invoke(&OmoriMewO::updateAnimation);
}

void IngameInterface::handleLevelSpecifics() const {
    switch (IngameMapHandler::instance->getLevel()) {
        case level::Name::kLevelWhiteSpace:
            handleLevelSpecifics_kLevelWhiteSpace();
            break;

        default: break;
    }
}

void IngameInterface::handleEntitiesSFX() const {
    Player::invoke(&Player::handleSFX);
    // Teleporter::invoke(&Teleporter::handleSFX);
    Slime::invoke(&Slime::handleSFX);
}

void IngameInterface::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) const {
    auto data = event::getData<event::Data_Teleporter>(event);
    
    IngameMapHandler::invoke(&IngameMapHandler::changeLevel, data.targetLevel);
    // globals::currentLevelData.playerLevelData.destCoords = data.targetDestCoords;   // This has no effect if placed here
    if (mCachedTargetDestCoords != nullptr) delete mCachedTargetDestCoords;
    mCachedTargetDestCoords = new SDL_Point(data.targetDestCoords);   // Cache

    // globals::state = GameState::kLoading | GameState::kIngamePlaying;
    onLevelChange();
    onWindowChange();
}

void IngameInterface::handleCustomEventGET_kReq_DeathPending_Player() const {
    IngameMapHandler::instance->isOnGrayscale = true;
}

void IngameInterface::handleCustomEventGET_kReq_DeathFinalized_Player() const {
    globals::state = GameState::kGameOver;
    IngameMapHandler::instance->isOnGrayscale = false;
}

void IngameInterface::handleLevelSpecifics_kLevelWhiteSpace() const {
    static unsigned int borderTraversedTracker = 0;
    static constexpr auto kArbitraryClamp = [&](int& i, const double lower, const double upper) {
        if (i <= lower) {
            if (!borderTraversedTracker) ++borderTraversedTracker;
            i = upper;
        } else if (i >= upper) {
            if (!borderTraversedTracker) ++borderTraversedTracker;
            i = lower;
        }
        // return (i <= lower) ? upper : (i >= upper) ? lower : i;
    };

    // "Infinite loop" effect
    if (Player::instance->pNextDestCoords != nullptr) {
        kArbitraryClamp(Player::instance->pNextDestCoords->x, IngameViewHandler::instance->mTileCountWidth / 2 + 1, level::data.tileDestCount.x - IngameViewHandler::instance->mTileCountWidth / 2 - 1);
        kArbitraryClamp(Player::instance->pNextDestCoords->y, IngameViewHandler::instance->mTileCountHeight / 2 + 2, level::data.tileDestCount.y - IngameViewHandler::instance->mTileCountHeight / 2 - 1);   // Slight deviation to prevent "staggering"
    }

    if (borderTraversedTracker == 1) {
        // Hard-coded unfortunately, will have to change in future commits
        auto data = new level::Data_Teleporter();
        data->destCoords = { 52, 43 };
        data->targetDestCoords = { 20, 8 };
        data->targetLevel = level::Name::kLevelEquilibrium;
        level::data.insert(config::entities::teleporter_red_hand_throne::typeID, data);
        RedHandThroneTeleporter::onLevelChangeAll(level::data.get(config::entities::teleporter_red_hand_throne::typeID));
        RedHandThroneTeleporter::invoke(&RedHandThroneTeleporter::onWindowChange);

        ++borderTraversedTracker;
    }
}