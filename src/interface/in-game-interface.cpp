#include <interface.hpp>

#include <array>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    static constexpr auto renderIngameDependencies = []() {
        // Static assets
        IngameMapHandler::invoke(&IngameMapHandler::render);

        // Non-interactible entities
        OmoriLightBulb::invoke(&OmoriLightBulb::render);
        OmoriKeysWASD::invoke(&OmoriKeysWASD::render);
        HospitalXRayMachine::invoke(&HospitalXRayMachine::render);

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
        HospitalXRayScreenArm::invoke(&HospitalXRayScreenArm::render);
        HospitalXRayScreenHead::invoke(&HospitalXRayScreenHead::render);
        HospitalSink::invoke(&HospitalSink::render);

        // Entities
        // PlaceholderTeleporter::invoke(&PlaceholderTeleporter::render);
        RedHandThrone::invoke(&RedHandThrone::render);

        Slime::invoke(&Slime::render);
        PixelCatGray::invoke(&PixelCatGray::render);
        PixelCatGold::invoke(&PixelCatGold::render);

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
    OmoriKeysWASD::deinitialize();
    HospitalXRayMachine::deinitialize();

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
    HospitalXRayScreenArm::deinitialize();
    HospitalXRayScreenHead::deinitialize();
    HospitalSink::deinitialize();

    PlaceholderTeleporter::deinitialize();
    RedHandThrone::deinitialize();

    Slime::deinitialize();
    PixelCatGray::deinitialize();
    PixelCatGold::deinitialize();

    PentacleProjectile::deinitialize();

    IngameDialogueBox::deinitialize();
}

void IngameInterface::initialize() {
    IngameMapHandler::initialize();

    Player::initialize();

    OmoriLightBulb::initialize();
    OmoriKeysWASD::initialize();
    HospitalXRayMachine::initialize();

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
    HospitalXRayScreenArm::initialize();
    HospitalXRayScreenHead::initialize();
    HospitalSink::initialize();

    // PlaceholderTeleporter::initialize();
    RedHandThrone::initialize();

    Slime::initialize();
    PixelCatGray::initialize();
    PixelCatGold::initialize();

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

    // Populate `level::data.properties` members
    switch (IngameMapHandler::instance->getLevel()) {
        case level::Name::kLevelWhiteSpace:
            level::data.setProperty<bool>("is-border-traversed", false);
            break;

        case level::Name::kLevelDeprecatedTutorial_0:
        case level::Name::kLevelDeprecatedTutorial_1:
            level::data.setProperty<int>("progress", -config::game::FPS >> 1);   // Magic number
            level::data.setProperty<int>("progress-arb-1", 0);
            level::data.setProperty<int>("progress-arb-2", 0);
            level::data.setProperty<int>("progress-arb-3", 0);
            break;

        default: break;
    }
    
    if (mCachedTargetDestCoords != nullptr) {
        auto data = new level::Data_Generic();
        data->destCoords = *mCachedTargetDestCoords;
        level::data.insert(config::entities::player::typeID, data);
    }

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, *(level::data.get(config::entities::player::typeID)[0]));

    OmoriLightBulb::onLevelChangeAll(level::data.get(config::entities::omori_light_bulb::typeID));
    OmoriKeysWASD::onLevelChangeAll(level::data.get(config::entities::omori_keys_wasd::typeID));
    HospitalXRayMachine::onLevelChangeAll(level::data.get(config::entities::hospital_xray_machine::typeID));

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
    HospitalXRayScreenArm::onLevelChangeAll(level::data.get(config::entities::hospital_xray_screen_arm::typeID));
    HospitalXRayScreenHead::onLevelChangeAll(level::data.get(config::entities::hospital_xray_screen_head::typeID));
    HospitalSink::onLevelChangeAll(level::data.get(config::entities::hospital_sink::typeID));

    PlaceholderTeleporter::onLevelChangeAll(level::data.get(config::entities::placeholder_teleporter::typeID));
    RedHandThrone::onLevelChangeAll(level::data.get(config::entities::teleporter_red_hand_throne::typeID));

    Slime::onLevelChangeAll(level::data.get(config::entities::slime::typeID));
    PixelCatGray::onLevelChangeAll(level::data.get(config::entities::pixel_cat_gray::typeID));
    PixelCatGold::onLevelChangeAll(level::data.get(config::entities::pixel_cat_gold::typeID));

    PentacleProjectile::onLevelChangeAll();

    Mixer::invoke(&Mixer::onLevelChange, IngameMapHandler::instance->getLevel());   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    IngameMapHandler::invoke(&IngameMapHandler::onWindowChange);
    IngameViewHandler::invoke(&IngameViewHandler::onWindowChange);

    Player::invoke(&Player::onWindowChange);

    OmoriLightBulb::invoke(&OmoriLightBulb::onWindowChange);
    OmoriKeysWASD::invoke(&OmoriKeysWASD::onWindowChange);
    HospitalXRayMachine::invoke(&HospitalXRayMachine::onWindowChange);

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
    HospitalXRayScreenArm::invoke(&HospitalXRayScreenArm::onWindowChange);
    HospitalXRayScreenHead::invoke(&HospitalXRayScreenHead::onWindowChange);
    HospitalSink::invoke(&HospitalSink::onWindowChange);

    PlaceholderTeleporter::invoke(&PlaceholderTeleporter::onWindowChange);
    RedHandThrone::invoke(&RedHandThrone::onWindowChange);

    Slime::invoke(&Slime::onWindowChange);
    PixelCatGray::invoke(&PixelCatGray::onWindowChange);
    PixelCatGold::invoke(&PixelCatGold::onWindowChange);

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
    HospitalXRayScreenArm::invoke(&HospitalXRayScreenArm::handleCustomEventGET, event);
    HospitalXRayScreenHead::invoke(&HospitalXRayScreenHead::handleCustomEventGET, event);
    HospitalSink::invoke(&HospitalSink::handleCustomEventGET, event);

    PlaceholderTeleporter::invoke(&PlaceholderTeleporter::handleCustomEventGET, event);
    RedHandThrone::invoke(&RedHandThrone::handleCustomEventGET, event);

    Slime::invoke(&Slime::handleCustomEventGET, event);
    PixelCatGray::invoke(&PixelCatGray::handleCustomEventGET, event);
    PixelCatGold::invoke(&PixelCatGold::handleCustomEventGET, event);

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
    OmoriKeysWASD::invoke(&OmoriKeysWASD::updateAnimation);
    HospitalXRayMachine::invoke(&HospitalXRayMachine::updateAnimation);

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
    HospitalXRayScreenArm::invoke(&HospitalXRayScreenArm::updateAnimation);
    HospitalXRayScreenHead::invoke(&HospitalXRayScreenHead::updateAnimation);
    HospitalSink::invoke(&HospitalSink::updateAnimation);

    // PlaceholderTeleporter::invoke(&PlaceholderTeleporter::updateAnimation);
    RedHandThrone::invoke(&RedHandThrone::updateAnimation);

    Slime::invoke(&Slime::move);
    Slime::invoke(&Slime::updateAnimation);
    PixelCatGray::invoke(&PixelCatGray::move);
    PixelCatGray::invoke(&PixelCatGray::updateAnimation);
    PixelCatGold::invoke(&PixelCatGold::move);
    PixelCatGold::invoke(&PixelCatGold::updateAnimation);

    PentacleProjectile::invoke(&PentacleProjectile::handleInstantiation);
    PentacleProjectile::invoke(&PentacleProjectile::updateAnimation);

    Player::invoke(&Player::move);
    Player::invoke(&Player::updateAnimation);
}

void IngameInterface::handleLevelSpecifics() const {
    #define IMPL(ln) \
    case ln:\
        handleLevelSpecifics_impl<ln>();\
        break;

    switch (IngameMapHandler::instance->getLevel()) {
        IMPL(level::Name::kLevelWhiteSpace)
        IMPL(level::Name::kLevelDeprecatedTutorial_0)
        IMPL(level::Name::kLevelDeprecatedTutorial_1)
        IMPL(level::Name::kLevelForest_0)
        IMPL(level::Name::kLevelForest_1)
        IMPL(level::Name::kLevelForest_2)
        IMPL(level::Name::kLevelForest_3)
        IMPL(level::Name::kLevelForest_4)

        default: break;
    }
}

void IngameInterface::handleEntitiesSFX() const {
    Player::invoke(&Player::handleSFX);
    // Teleporter::invoke(&Teleporter::handleSFX);
    Slime::invoke(&Slime::handleSFX);
    PixelCatGray::invoke(&PixelCatGray::handleSFX);
    PixelCatGold::invoke(&PixelCatGold::handleSFX);
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
typename std::enable_if_t<L == level::Name::kLevelDeprecatedTutorial_0>
IngameInterface::handleLevelSpecifics_impl() const {
    static auto proceed = [&](std::string const& key = "progress") {
        level::data.setProperty<int>(key, level::data.getProperty<int>(key) + 1);
    };

    switch (level::data.getProperty<int>("progress")) {
        case 0:   // At the very beginning
            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Proceed with [E].)",
                "... (Move with [W][A][S][D].)",
            });

            proceed();
            break;

        case 1:   // Before leaving the first platform
            if (!isPlayerInRange({ -1, -1 }, { -1, 89 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (You can try talking to the cat up there.)",
            });

            proceed();
            break;

        case 2:   // Upon entering the first arch door
            if (!isPlayerInRange({ 23, 24 }, { 88, 89 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (You can try talking to the cat on your left.)",
            });

            proceed();
            break;

        case 3:   // Upon entering the second arch door
            if (!isPlayerInRange({ 39, 40 }, { 76, 77 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Enemy approaching)",
            });
            Mixer::invoke(&Mixer::stopBGM);

            proceed();
            break;

        case 4:   // Pillars 1
            if (!isPlayerInRange({ -1, 36 }, { 67, 73 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Press [SPACE] to not die.)",
            });
            Slime::instantiateEx({
                new level::Data_Generic({ 27, 70 }),
            });
            Mixer::invoke(&Mixer::onLevelChange, level::Name::__kLevelTutorial_0_Combat__);

            proceed();
            break;

        case 5:   // Pillars 2
            if (!isPlayerInRange({ -1, 27 }, { 67, 73 })) break;

            if (!Slime::isAllDead()) {
                switch (level::data.getProperty<int>("progress-arb-1")) {
                    case 0:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (You have to, like, repeatedly hit that moving thing first.)",
                            "... (Till it stops moving.)",
                        });
                        proceed("progress-arb-1");
                        break;

                    case 1:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (Having difficulties? Try pressing [1].)",
                        });
                        break;

                    default: break;
                } break;
            }

            Slime::instantiateEx({
                new level::Data_Generic({ 17, 68 }),
                new level::Data_Generic({ 18, 70 }),
                new level::Data_Generic({ 17, 72 }),
            });

            proceed();
            break;

        case 6:   // Pillars 3
            if (!isPlayerInRange({ -1, 18 }, { 67, 73 })) break;

            if (!Slime::isAllDead()) {
                switch (level::data.getProperty<int>("progress-arb-2")) {
                    case 0:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (You have to, like, repeatedly hit those moving things first.)",
                            "... (Till they stop moving.)",
                        });
                        proceed("progress-arb-2");
                        break;

                    case 1:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (Having difficulties? Try pressing [1].)",
                            "... (And, uh, other numbers too, of course. Some might just work, y'know.)",
                        });
                        proceed("progress-arb-2");
                        break;

                    case 2:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (Try other numbers too. Some might just work, y'know.)",
                        });
                        break;

                    default: break;
                } break;
            }

            Slime::instantiateEx({
                new level::Data_Generic({ 8, 68 }),
                new level::Data_Generic({ 9, 70 }),
                new level::Data_Generic({ 8, 72 }),
                new level::Data_Generic({ 28, 68 }),
                new level::Data_Generic({ 27, 70 }),
                new level::Data_Generic({ 28, 68 }),
            });

            proceed();
            break;

        case 7:   // Pillars 4
            if (!Slime::isAllDead()) {
                if (!isPlayerInRange({ -1, 9 }, { 67, 73 })) break;
                switch (level::data.getProperty<int>("progress-arb-3")) {
                    case 0:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (You have to, like, repeatedly hit those moving things first.)",
                            "... (Till they stop moving.)",
                        });
                        proceed("progress-arb-3");
                        break;

                    case 1:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Uh, don't do that.)",
                            "... (C'mon, we've gone through this so many times already.)",
                        });
                        proceed("progress-arb-3");
                        break;

                    case 2:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Try that one more time and you shall know pain.)",
                        });
                        proceed("progress-arb-3");
                        break;

                    case 3: {
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (Like, really?)",
                        });

                        std::vector<level::Data_Generic*> instantiationData;
                        for (int y = 66; y <= 74; ++y) for (int x = 2; x <= 8; ++x) instantiationData.push_back(new level::Data_Generic({ x, y }));
                        Slime::instantiateEx(instantiationData);

                        proceed("progress-arb-3");
                    } break;

                    case 4:
                        IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                            "... (...)",
                        });
                        break;

                    default: break;
                } break;
            }

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Stand proud,)",
                "... (You've done well.)",
            });
            Mixer::invoke(&Mixer::onLevelChange, level::Name::kLevelDeprecatedTutorial_0);
            
            proceed();
            break;

        default:
            if (level::data.getProperty<int>("progress") < 0) proceed();
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelDeprecatedTutorial_1>
IngameInterface::handleLevelSpecifics_impl() const {
    static int progress;
    progress = level::data.getProperty<int>("progress");
    static auto proceed = [&]() {
        level::data.setProperty<int>("progress", ++progress);
    };

    switch (progress) {
        case 0:
            if (!isPlayerInRange({ 46, 48 }, { -1, 20 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Up next: some OMORI reference)",
            });
            Mixer::invoke(&Mixer::stopBGM);

            proceed();
            break;

        default:
            if (progress < 0) proceed();
    }
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

    if (isBorderTraversed && RedHandThrone::instances.empty()) RedHandThrone::instantiateEx({
        new level::Data_Teleporter{ { 52, 43 }, { 20, 11 }, level::Name::kLevelBreakroomInitial },
    });
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelForest_0>
IngameInterface::handleLevelSpecifics_impl() const {
    if (PlaceholderTeleporter::instances.empty()) {
        std::vector<level::Data_Generic*> data;
        for (int y = 4; y <= 18; ++y) data.push_back(new level::Data_Teleporter(
            { 49, y }, { 1, y }, level::Name::kLevelForest_1
        ));
        PlaceholderTeleporter::instantiateEx(data);
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelForest_1>
IngameInterface::handleLevelSpecifics_impl() const {
    if (PlaceholderTeleporter::instances.empty()) {
        std::vector<level::Data_Generic*> data;
        for (int y = 4; y <= 18; ++y) {
            data.push_back(new level::Data_Teleporter( { 0, y }, { 48, y }, level::Name::kLevelForest_0 ));
            data.push_back(new level::Data_Teleporter( { 49, y }, { 1, y }, level::Name::kLevelForest_2 ));
        }
        PlaceholderTeleporter::instantiateEx(data);
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelForest_2>
IngameInterface::handleLevelSpecifics_impl() const {
    if (PlaceholderTeleporter::instances.empty()) {
        std::vector<level::Data_Generic*> data;
        for (int y = 4; y <= 18; ++y) {
            data.push_back(new level::Data_Teleporter( { 0, y }, { 48, y }, level::Name::kLevelForest_1 ));
            data.push_back(new level::Data_Teleporter( { 49, y }, { 1, y }, level::Name::kLevelForest_3 ));
        }
        PlaceholderTeleporter::instantiateEx(data);
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelForest_3>
IngameInterface::handleLevelSpecifics_impl() const {
    if (PlaceholderTeleporter::instances.empty()) {
        std::vector<level::Data_Generic*> data;
        for (int y = 4; y <= 18; ++y) {
            data.push_back(new level::Data_Teleporter( { 0, y }, { 48, y }, level::Name::kLevelForest_2 ));
            data.push_back(new level::Data_Teleporter( { 49, y }, { 1, y }, level::Name::kLevelForest_4 ));
        }
        PlaceholderTeleporter::instantiateEx(data);
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelForest_4>
IngameInterface::handleLevelSpecifics_impl() const {
    if (PlaceholderTeleporter::instances.empty()) {
        std::vector<level::Data_Generic*> data;
        for (int y = 4; y <= 18; ++y) data.push_back(new level::Data_Teleporter(
            { 0, y }, { 48, y }, level::Name::kLevelForest_3
        ));
        for (int x = 3; x <= 26; ++x) data.push_back(new level::Data_Teleporter(
            { x, 74 }, { 54, 49 }, level::Name::kLevelWhiteSpace
        ));
        PlaceholderTeleporter::instantiateEx(data);
    }
}