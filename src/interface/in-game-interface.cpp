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

    // Populate `level::data.properties` members
    switch (IngameMapHandler::instance->getLevel()) {
        case level::Name::kLevelWhiteSpace:
            level::data.setProperty<bool>("is-border-traversed", false);
            break;

        case level::Name::kLevelTutorial_0:
        case level::Name::kLevelTutorial_1:
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

        case level::Name::kLevelTutorial_0:
            handleLevelSpecifics_kLevelTutorial_0();
            break;

        case level::Name::kLevelTutorial_1:
            handleLevelSpecifics_kLevelTutorial_1();
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

bool IngameInterface::isPlayerInRange(std::pair<int, int> const& x_lim, std::pair<int, int> const& y_lim) const {
    bool result = Player::instance->isWithinRange(x_lim, y_lim);

    if (result) {
        Player::invoke(&Player::onMoveEnd, EntityStatus::kInvalidated);
        Player::invoke(&Player::onRunningToggled, false);
    }

    return result;
}

void IngameInterface::handleLevelSpecifics_kLevelTutorial_0() const {
    static auto proceed = [&](std::string const& key = "progress") {
        level::data.setProperty<int>(key, level::data.getProperty<int>(key) + 1);
    };

    switch (level::data.getProperty<int>("progress")) {
        case 0:   // At the very beginning
            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Press [E]. That's all you need to know.)\n(For now.)",
                "... (Good job! Now...)",
                "... (Did you know you could move around with [W][A][S][D]?)",
                "... (C'mon, give it a little try!)",
            });

            proceed();
            break;

        case 1:   // Before leaving the first platform
            if (!isPlayerInRange({ -1, -1 }, { -1, 89 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Pssst, hey you!)",
                "... (Yes, you!)",
                "... (Do you see that cat up there?)",
                "... (It says it wanna talk to you!)",
                "... (So, uh, just go there, I guess?)",
                "... (You don't want to disappoint a cat, do you?)",
            });

            proceed();
            break;

        case 2:   // Upon entering the first arch door
            if (!isPlayerInRange({ 23, 24 }, { 88, 89 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Pssst, hey you!)",
                "... (Yes, you again! Like, who else is here?)",
                "... (Do you see that cat on your left?)",
                "... (It says it wanna talk to you!)",
                "... (So, uh, just go there, I guess?)",
                "... (You don't want to disappoint a cat, do you?)",
            });

            proceed();
            break;

        case 3:   // Upon entering the second arch door
            if (!isPlayerInRange({ 39, 40 }, { 76, 77 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (Pssst, hey you!)",
                "... (I hope you talked to both cats.)",
                "... (Like, the developer who created all this didn't really provide me with a proper mechanism to track such interactions, so I really do not know.)",
                "... (But, uh, if you haven't, can you, like, go back and talk to them?)",
                "... (Because, uh, you'll never see them again? C'mon, everyone knows that this tutorial is a one-time thing.)",
                "...",
                "... (Oh, one more thing.)"
                "... (Make sure you read the sign before going any further.)",
            });
            Mixer::invoke(&Mixer::stopBGM);

            proceed();
            break;

        case 4:   // Pillars 1
            if (!isPlayerInRange({ -1, 36 }, { 67, 73 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (You might want to press [SPACE].)",
                "... (Because, uh, you might be dead otherwise.)",
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
            Mixer::invoke(&Mixer::onLevelChange, level::Name::kLevelTutorial_0);
            
            proceed();
            break;

        default:
            if (level::data.getProperty<int>("progress") < 0) proceed();
    }
}

void IngameInterface::handleLevelSpecifics_kLevelTutorial_1() const {
    static int progress;
    progress = level::data.getProperty<int>("progress");
    static auto proceed = [&]() {
        level::data.setProperty<int>("progress", ++progress);
    };

    switch (progress) {
        case 0:
            if (!isPlayerInRange({ 46, 48 }, { -1, 20 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "... (If you truly wish to leave the TUTORIAL...)",
                "... (I will not stop you.)",
                "... (However, when you leave...)",
                "... (Please do not come back.)",
                "... (I hope you understand.)",
                "... (Goodbye, my child.)",
            });
            Mixer::invoke(&Mixer::stopBGM);

            proceed();
            break;

        case 1:
            if (!isPlayerInRange({ 46, 48 }, { -1, 2 })) break;

            IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>{
                "...",
            });

            proceed();
            break;

        default:
            if (progress < 0) proceed();
    }
}

void IngameInterface::handleLevelSpecifics_kLevelWhiteSpace() const {
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
    if (Player::instance->pNextDestCoords != nullptr) {
        kInternalTeleportHandler(Player::instance->pNextDestCoords->x, IngameViewHandler::instance->mTileCountWidth / 2 + 1, level::data.tileDestCount.x - IngameViewHandler::instance->mTileCountWidth / 2 - 1);
        kInternalTeleportHandler(Player::instance->pNextDestCoords->y, IngameViewHandler::instance->mTileCountHeight / 2 + 2, level::data.tileDestCount.y - IngameViewHandler::instance->mTileCountHeight / 2 - 1);   // Slight deviation to prevent "staggering"
    }

    if (isBorderTraversed && RedHandThrone::instances.empty()) RedHandThrone::instantiateEx({
        new level::Data_Teleporter{ { 52, 43 }, { 20, 11 }, level::Name::kLevelBreakroomInitial },
    });
}