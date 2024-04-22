#include <interface.hpp>

#include <array>
#include <fstream>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    static constexpr auto renderIngameDependencies = []() {
        Invoker<IngameMapHandler, ABSTRACT_ANIMATED_ENTITY, GENERIC_INTERACTABLE, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE, Player>::invoke_render();
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
    Invoker<IngameMapHandler, IngameViewHandler, Player, ABSTRACT_ANIMATED_ENTITY, PlaceholderInteractable, GENERIC_INTERACTABLE, PlaceholderTeleporter, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE, IngameDialogueBox>::invoke_deinitialize();
}

void IngameInterface::initialize() {
    Invoker<IngameMapHandler, Player, ABSTRACT_ANIMATED_ENTITY, PlaceholderInteractable, GENERIC_INTERACTABLE, PlaceholderTeleporter, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE>::invoke_initialize();
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
        level::data.erase(config::entities::player::typeID);
        level::data.insert(config::entities::player::typeID, data);
    }

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    auto playerLevelData = level::data.get(config::entities::player::typeID);
    if (!playerLevelData.empty() && playerLevelData.front() != nullptr) Player::invoke(&Player::onLevelChange, *playerLevelData.front());

    Invoker<ABSTRACT_ANIMATED_ENTITY, PlaceholderInteractable, GENERIC_INTERACTABLE, PlaceholderTeleporter, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE>::invoke_onLevelChangeAll();
    Mixer::invoke(&Mixer::onLevelChange, levelName);   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    Invoker<IngameMapHandler, IngameViewHandler, Player, ABSTRACT_ANIMATED_ENTITY, GENERIC_INTERACTABLE, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE, IngameDialogueBox>::invoke_onWindowChange();
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

    Invoker<Player, PlaceholderInteractable, GENERIC_INTERACTABLE, PlaceholderTeleporter, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE>::invoke_handleCustomEventGET(event);
}

/**
 * @note `GameState::kIngamePlaying` only.
*/
void IngameInterface::handleCustomEventPOST() const {
    Invoker<Player, PlaceholderTeleporter, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE>::invoke_handleCustomEventPOST();
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
    Invoker<ABSTRACT_ANIMATED_ENTITY, GENERIC_INTERACTABLE, GENERIC_TELEPORTER_ENTITY, GENERIC_HOSTILE_ENTITY, GENERIC_SURGE_PROJECTILE, Player>::invoke_updateAnimation();
    Invoker<GENERIC_SURGE_PROJECTILE>::invoke_handleInstantiation();
    Player::invoke(&Player::handleAutopilotMovement);   // Autopilot
    Invoker<GENERIC_HOSTILE_ENTITY, Player>::invoke_move();
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
    Invoker<Player, GENERIC_HOSTILE_ENTITY>::invoke_handleSFX();
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