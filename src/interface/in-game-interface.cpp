#include <interface.hpp>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    auto renderIngameDependencies = []() {
        IngameMapHandler::invoke(&IngameMapHandler::render);

        Teleporter::invoke(&Teleporter::render);
        Slime::invoke(&Slime::render);
        Player::invoke(&Player::render);

        PentacleProjectile::invoke(&PentacleProjectile::render);
        HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::render);
    };

    Player::instantiate(SDL_Point{ 0, 0 });   // This is required for below instantiations
    IngameMapHandler::instantiate(config::interface::levelName);
    IngameViewHandler::instantiate(renderIngameDependencies, Player::instance->destRect, IngameViewMode::kFocusOnEntity);
}

/**
 * @note Intellisense's `more than one instance of overloaded function "[...]::deinitialize" matches the argument list:C/C++(308)` could be safely ignored as no relevant warnings are shown during compilation.
*/
void IngameInterface::deinitialize() {
    IngameMapHandler::deinitialize();
    IngameViewHandler::deinitialize();

    PentacleProjectile::deinitialize();
    HauntedBookcaseProjectile::deinitialize();

    Player::deinitialize();
    Teleporter::deinitialize();
    Slime::deinitialize();
}

void IngameInterface::initialize() {
    IngameMapHandler::initialize();

    PentacleProjectile::initialize();
    HauntedBookcaseProjectile::initialize();

    Player::initialize();
    Teleporter::initialize();
    Slime::initialize();
}

void IngameInterface::render() const {
    IngameViewHandler::invoke(&IngameViewHandler::render);
}

void IngameInterface::onLevelChange() const {
    // Populate `globals::currentLevelData` members
    IngameMapHandler::invoke(&IngameMapHandler::onLevelChange);

    PentacleProjectile::onLevelChangeAll();
    HauntedBookcaseProjectile::onLevelChangeAll();

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, globals::currentLevelData.playerLevelData);
    Teleporter::onLevelChangeAll(globals::currentLevelData.teleportersLevelData);
    Slime::onLevelChangeAll(globals::currentLevelData.slimesLevelData);

    Mixer::invoke(&Mixer::onLevelChange, IngameMapHandler::instance->getLevel());   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    IngameMapHandler::invoke(&IngameMapHandler::onWindowChange);
    IngameViewHandler::invoke(&IngameViewHandler::onWindowChange);

    PentacleProjectile::invoke(&PentacleProjectile::onWindowChange);
    HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::onWindowChange);

    Player::invoke(&Player::onWindowChange);
    Teleporter::invoke(&Teleporter::onWindowChange);
    Slime::invoke(&Slime::onWindowChange);
}

void IngameInterface::handleKeyBoardEvent(SDL_Event const& event) const {
    switch (event.key.keysym.sym) {
        case config::key::INGAME_RETURN_MENU:
            if (event.type != SDL_KEYDOWN) break;
            globals::state = GameState::kLoading | GameState::kMenu;
            break;

        case config::key::INGAME_LEVEL_RESET:
            if (event.type != SDL_KEYDOWN) break;
            onLevelChange(); onWindowChange();
            break;

        default: break;
    }

    IngameViewHandler::invoke(&IngameViewHandler::handleKeyBoardEvent, event);
    IngameMapHandler::invoke(&IngameMapHandler::handleKeyBoardEvent, event);
    Player::invoke(&Player::handleKeyboardEvent, event);
}

void IngameInterface::handleMouseEvent(SDL_Event const& event) const {
    // if (event.type != SDL_MOUSEBUTTONDOWN) return;
    // onLevelChange(); onWindowChange();
}

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
    HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::handleCustomEventGET, event);

    Player::invoke(&Player::handleCustomEventGET, event);
    Teleporter::invoke(&Teleporter::handleCustomEventGET, event);
    Slime::invoke(&Slime::handleCustomEventGET, event);
}

void IngameInterface::handleCustomEventPOST() const {
    PentacleProjectile::invoke(&PentacleProjectile::handleCustomEventPOST);
    HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::handleCustomEventPOST);

    Player::invoke(&Player::handleCustomEventPOST);
    Teleporter::invoke(&Teleporter::handleCustomEventPOST);
    Slime::invoke(&Slime::handleCustomEventPOST);
}

void IngameInterface::handleEntities() const {
    handleEntitiesMovement();
    // handleEntitiesInteraction();
    handleEntitiesSFX();
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void IngameInterface::handleEntitiesMovement() const {
    PentacleProjectile::invoke(&PentacleProjectile::handleInstantiation);
    PentacleProjectile::handleTermination();
    PentacleProjectile::invoke(&PentacleProjectile::updateAnimation);

    HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::move);
    HauntedBookcaseProjectile::invoke(&HauntedBookcaseProjectile::updateAnimation);

    Player::invoke(&Player::initiateAnimation);
    Player::invoke(&Player::move);
    Player::invoke(&Player::updateAnimation);

    Teleporter::invoke(&Teleporter::updateAnimation);

    Slime::invoke(&Slime::initiateAnimation);
    Slime::invoke(&Slime::move);
    Slime::invoke(&Slime::updateAnimation);
}

void IngameInterface::handleEntitiesSFX() const {
    Player::invoke(&Player::handleSFX);
    // Teleporter::invoke(&Teleporter::handleSFX);
    Slime::invoke(&Slime::handleSFX);
}

void IngameInterface::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) const {
    auto data = *reinterpret_cast<event::data::Teleporter*>(event.user.data1);
    
    IngameMapHandler::invoke(&IngameMapHandler::changeLevel, data.targetLevel);
    globals::currentLevelData.playerLevelData.destCoords = data.targetDestCoords;

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