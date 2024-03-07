#include <interface.hpp>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    static constexpr auto renderIngameDependencies = []() {
        // Static assets
        IngameMapHandler::invoke(&IngameMapHandler::render);

        // Non-interactible entities
        OmoriLightBulb::invoke(&OmoriLightBulb::render);

        // Interactables
        // PlaceholderInteractable::invoke(&PlaceholderInteractable::render);
        OmoriLaptop::invoke(&OmoriLaptop::render);
        OmoriMewO::invoke(&OmoriMewO::render);
        OmoriCat_0::invoke(&OmoriCat_0::render);
        OmoriCat_1::invoke(&OmoriCat_1::render);
        OmoriCat_2::invoke(&OmoriCat_2::render);
        OmoriCat_3::invoke(&OmoriCat_3::render);
        OmoriCat_4::invoke(&OmoriCat_4::render);
        OmoriCat_5::invoke(&OmoriCat_5::render);
        OmoriCat_6::invoke(&OmoriCat_6::render);
        OmoriCat_7::invoke(&OmoriCat_7::render);

        // Entities
        // PlaceholderTeleporter::invoke(&PlaceholderTeleporter::render);
        RedHandThrone::invoke(&RedHandThrone::render);

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

    Player::deinitialize();

    OmoriLightBulb::deinitialize();

    PlaceholderInteractable::deinitialize();
    OmoriLaptop::deinitialize();
    OmoriMewO::deinitialize();
    OmoriCat_0::deinitialize();
    OmoriCat_1::deinitialize();
    OmoriCat_2::deinitialize();
    OmoriCat_3::deinitialize();
    OmoriCat_4::deinitialize();
    OmoriCat_5::deinitialize();
    OmoriCat_6::deinitialize();
    OmoriCat_7::deinitialize();

    PlaceholderTeleporter::deinitialize();
    RedHandThrone::deinitialize();

    Slime::deinitialize();

    PentacleProjectile::deinitialize();

    IngameDialogueBox::deinitialize();
}

void IngameInterface::initialize() {
    IngameMapHandler::initialize();

    Player::initialize();

    OmoriLightBulb::initialize();

    PlaceholderInteractable::initialize();
    OmoriMewO::initialize();
    OmoriLaptop::initialize();
    OmoriCat_0::initialize();
    OmoriCat_1::initialize();
    OmoriCat_2::initialize();
    OmoriCat_3::initialize();
    OmoriCat_4::initialize();
    OmoriCat_5::initialize();
    OmoriCat_6::initialize();
    OmoriCat_7::initialize();

    // PlaceholderTeleporter::initialize();
    RedHandThrone::initialize();

    Slime::initialize();

    PentacleProjectile::initialize();
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

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, *(level::data.get(config::entities::player::typeID)[0]));

    OmoriLightBulb::onLevelChangeAll(level::data.get(config::entities::omori_light_bulb::typeID));

    PlaceholderInteractable::onLevelChangeAll(level::data.get(config::entities::placeholder_interactable::typeID));
    OmoriLaptop::onLevelChangeAll(level::data.get(config::entities::omori_laptop::typeID));
    OmoriMewO::onLevelChangeAll(level::data.get(config::entities::omori_mewo::typeID));
    OmoriCat_0::onLevelChangeAll(level::data.get(config::entities::omori_cat_0::typeID));
    OmoriCat_1::onLevelChangeAll(level::data.get(config::entities::omori_cat_1::typeID));
    OmoriCat_2::onLevelChangeAll(level::data.get(config::entities::omori_cat_2::typeID));
    OmoriCat_3::onLevelChangeAll(level::data.get(config::entities::omori_cat_3::typeID));
    OmoriCat_4::onLevelChangeAll(level::data.get(config::entities::omori_cat_4::typeID));
    OmoriCat_5::onLevelChangeAll(level::data.get(config::entities::omori_cat_5::typeID));
    OmoriCat_6::onLevelChangeAll(level::data.get(config::entities::omori_cat_6::typeID));
    OmoriCat_7::onLevelChangeAll(level::data.get(config::entities::omori_cat_7::typeID));

    PlaceholderTeleporter::onLevelChangeAll(level::data.get(config::entities::placeholder_teleporter::typeID));
    RedHandThrone::onLevelChangeAll(level::data.get(config::entities::teleporter_red_hand_throne::typeID));

    Slime::onLevelChangeAll(level::data.get(config::entities::slime::typeID));

    PentacleProjectile::onLevelChangeAll();

    Mixer::invoke(&Mixer::onLevelChange, IngameMapHandler::instance->getLevel());   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    IngameMapHandler::invoke(&IngameMapHandler::onWindowChange);
    IngameViewHandler::invoke(&IngameViewHandler::onWindowChange);

    Player::invoke(&Player::onWindowChange);

    OmoriLightBulb::invoke(&OmoriLightBulb::onWindowChange);

    PlaceholderInteractable::invoke(&PlaceholderInteractable::onWindowChange);
    OmoriLaptop::invoke(&OmoriLaptop::onWindowChange);
    OmoriMewO::invoke(&OmoriMewO::onWindowChange);
    OmoriCat_0::invoke(&OmoriCat_0::onWindowChange);
    OmoriCat_1::invoke(&OmoriCat_1::onWindowChange);
    OmoriCat_2::invoke(&OmoriCat_2::onWindowChange);
    OmoriCat_3::invoke(&OmoriCat_3::onWindowChange);
    OmoriCat_4::invoke(&OmoriCat_4::onWindowChange);
    OmoriCat_5::invoke(&OmoriCat_5::onWindowChange);
    OmoriCat_6::invoke(&OmoriCat_6::onWindowChange);
    OmoriCat_7::invoke(&OmoriCat_7::onWindowChange);

    PlaceholderTeleporter::invoke(&PlaceholderTeleporter::onWindowChange);
    RedHandThrone::invoke(&RedHandThrone::onWindowChange);

    Slime::invoke(&Slime::onWindowChange);

    PentacleProjectile::invoke(&PentacleProjectile::onWindowChange);

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


    Player::invoke(&Player::handleCustomEventGET, event);

    // Dialogues!
    PlaceholderInteractable::invoke(&PlaceholderInteractable::handleCustomEventGET, event);
    OmoriLaptop::invoke(&OmoriLaptop::handleCustomEventGET, event);
    OmoriMewO::invoke(&OmoriMewO::handleCustomEventGET, event);
    OmoriCat_0::invoke(&OmoriCat_0::handleCustomEventGET, event);
    OmoriCat_1::invoke(&OmoriCat_1::handleCustomEventGET, event);
    OmoriCat_2::invoke(&OmoriCat_2::handleCustomEventGET, event);
    OmoriCat_3::invoke(&OmoriCat_3::handleCustomEventGET, event);
    OmoriCat_4::invoke(&OmoriCat_4::handleCustomEventGET, event);
    OmoriCat_5::invoke(&OmoriCat_5::handleCustomEventGET, event);
    OmoriCat_6::invoke(&OmoriCat_6::handleCustomEventGET, event);
    OmoriCat_7::invoke(&OmoriCat_7::handleCustomEventGET, event);

    PlaceholderTeleporter::invoke(&PlaceholderTeleporter::handleCustomEventGET, event);
    RedHandThrone::invoke(&RedHandThrone::handleCustomEventGET, event);

    Slime::invoke(&Slime::handleCustomEventGET, event);

    PentacleProjectile::invoke(&PentacleProjectile::handleCustomEventGET, event);
}

/**
 * @note `GameState::kIngamePlaying` only.
*/
void IngameInterface::handleCustomEventPOST() const {
    Player::invoke(&Player::handleCustomEventPOST);
    
    PlaceholderTeleporter::invoke(&PlaceholderTeleporter::handleCustomEventPOST);
    RedHandThrone::invoke(&RedHandThrone::handleCustomEventPOST);

    Slime::invoke(&Slime::handleCustomEventPOST);

    PentacleProjectile::invoke(&PentacleProjectile::handleCustomEventPOST);
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
    OmoriLightBulb::invoke(&OmoriLightBulb::updateAnimation);

    PlaceholderInteractable::invoke(&PlaceholderInteractable::updateAnimation);
    OmoriLaptop::invoke(&OmoriLaptop::updateAnimation);
    OmoriMewO::invoke(&OmoriMewO::updateAnimation);
    OmoriCat_0::invoke(&OmoriCat_0::updateAnimation);
    OmoriCat_1::invoke(&OmoriCat_1::updateAnimation);
    OmoriCat_2::invoke(&OmoriCat_2::updateAnimation);
    OmoriCat_3::invoke(&OmoriCat_3::updateAnimation);
    OmoriCat_4::invoke(&OmoriCat_4::updateAnimation);
    OmoriCat_5::invoke(&OmoriCat_5::updateAnimation);
    OmoriCat_6::invoke(&OmoriCat_6::updateAnimation);
    OmoriCat_7::invoke(&OmoriCat_7::updateAnimation);

    // PlaceholderTeleporter::invoke(&PlaceholderTeleporter::updateAnimation);
    RedHandThrone::invoke(&RedHandThrone::updateAnimation);

    Slime::invoke(&Slime::initiateAnimation);
    Slime::invoke(&Slime::move);
    Slime::invoke(&Slime::updateAnimation);

    PentacleProjectile::invoke(&PentacleProjectile::handleInstantiation);
    PentacleProjectile::handleTermination();
    PentacleProjectile::invoke(&PentacleProjectile::updateAnimation);

    Player::invoke(&Player::initiateAnimation);
    Player::invoke(&Player::move);
    Player::invoke(&Player::updateAnimation);
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
    auto borderTraversedTracker = level::data.getProperty<bool>("is-border-traversed");

    auto kArbitraryClamp = [&](int& i, const double lower, const double upper) {
        if (i <= lower) {
            if (!borderTraversedTracker) level::data.setProperty("is-border-traversed", "true");
            i = upper;
        } else if (i >= upper) {
            if (!borderTraversedTracker) level::data.setProperty("is-border-traversed", "true");
            i = lower;
        }
        // return (i <= lower) ? upper : (i >= upper) ? lower : i;
    };   // Declaring as `static` would yield `warning: storing the address of local variable ‘borderTraversedTracker’ in ‘kArbitraryClamp.IngameInterface::handleLevelSpecifics_kLevelWhiteSpace() const::<lambda(int&, double, double)>::<borderTraversedTracker capture>’ [-Wdangling-pointer=]`

    // "Infinite loop" effect
    if (Player::instance->pNextDestCoords != nullptr) {
        kArbitraryClamp(Player::instance->pNextDestCoords->x, IngameViewHandler::instance->mTileCountWidth / 2 + 1, level::data.tileDestCount.x - IngameViewHandler::instance->mTileCountWidth / 2 - 1);
        kArbitraryClamp(Player::instance->pNextDestCoords->y, IngameViewHandler::instance->mTileCountHeight / 2 + 2, level::data.tileDestCount.y - IngameViewHandler::instance->mTileCountHeight / 2 - 1);   // Slight deviation to prevent "staggering"
    }

    if (borderTraversedTracker && RedHandThrone::instances.empty()) {
        // Hard-coded unfortunately, will have to change in future commits
        auto data = new level::Data_Teleporter();
        data->destCoords = { 52, 43 };
        data->targetDestCoords = { 20, 11 };
        data->targetLevel = level::Name::kLevelBreakroomInitial;
        level::data.insert(config::entities::teleporter_red_hand_throne::typeID, data);
        RedHandThrone::onLevelChangeAll(level::data.get(config::entities::teleporter_red_hand_throne::typeID));
        RedHandThrone::invoke(&RedHandThrone::onWindowChange);
    }
}