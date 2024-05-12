#include <interface.hpp>

#include <array>
#include <fstream>

#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


std::size_t IngameInterface::ProgressHandler::get() const {
    auto ln = IngameMapHandler::instance->getLevel();
    auto it = mProgress.find(ln);
    return it != mProgress.end() ? it->second : 0;
}

void IngameInterface::ProgressHandler::set(std::size_t value) const {
    auto ln = IngameMapHandler::instance->getLevel();
    auto it = mProgress.find(ln);
    if (it == mProgress.end()) mProgress.insert(std::make_pair(ln, 0));
    else it->second = value;
}

void IngameInterface::ProgressHandler::loadfromjson(json const& data) const {
    auto progress_j = data.find("progress"); if (progress_j == data.end()) return;
    auto progress_v = progress_j.value(); if (!progress_v.is_object()) return;

    auto impl = [&](level::Name ln) -> void {
        auto ln_j = progress_v.find(std::to_string(static_cast<unsigned int>(ln))); if (ln_j == progress_v.end()) return;
        mProgress.insert(std::make_pair(ln, ln_j.value()));
    };

    impl(level::Name::kLevelPrelude);
    impl(level::Name::kLevelWoodsEntryPoint);
    impl(level::Name::kLevelWoodsLongLane);
    impl(level::Name::kLevelWoodsMysteryShack);
    impl(level::Name::kLevelWoodsCrossroadsFirst);
    impl(level::Name::kLevelWoodsDeadEnd);
    impl(level::Name::kLevelWoodsEnemyApproachingFirst);
    impl(level::Name::kLevelWoodsEnemyApproachingFinal);
    impl(level::Name::kLevelWoodsCrossroadsFinal);
    impl(level::Name::kLevelWoodsDestinedDeath);

    impl(level::Name::kLevelInterlude);

    impl(level::Name::kLevelWhiteSpace);
}

json& IngameInterface::ProgressHandler::savetojson(json& data) const {
    for (const auto& pair : mProgress) data["progress"][std::to_string(static_cast<unsigned int>(pair.first))] = pair.second;
    return data;
}

void IngameInterface::SaveHandler::loadfromfile() const {
    if (!std::filesystem::exists(config::interface::savePath)) return;

    json data;
    utils::fetch(mPath, data);
    if (data.empty()) return;

    auto ln = level::hstoln(data["level"]);
    if (ln.has_value()) IngameMapHandler::invoke(&IngameMapHandler::changeLevel, ln.value());

    mPL = std::make_optional<SDL_Point>({ data["player"]["x"], data["player"]["y"] });

    mProgress.clear();
    mProgress.loadfromjson(data);
}

void IngameInterface::SaveHandler::clear() const {
    IngameMapHandler::invoke(&IngameMapHandler::changeLevel, config::interface::levelName);
    mPL.reset();
    mProgress.clear();
}

json IngameInterface::SaveHandler::savetojson() const {
    json data;

    data["level"] = IngameMapHandler::instance->getLevel();
    data["player"]["x"] = Player::instance->mDestCoords.x;
    data["player"]["y"] = Player::instance->mDestCoords.y;

    mProgress.savetojson(data);

    return data;
}

void IngameInterface::SaveHandler::savetofile(json const& data) const {
    std::ofstream file;

    file.open(mPath, std::ofstream::out);
    if (!file.is_open()) return;

    file << data.dump(2, ' ', true, json::error_handler_t::strict);   // Should move into `config::interface` ?
    file.close();
}

IngameInterface::IngameInterface() : save(mProgress,  config::interface::savePath) {
    static constexpr auto renderIngameDependencies = []() {
        Invoker<IngameMapHandler, NON_INTERACTABLES, INTERACTABLES, TELEPORTERS, HOSTILES, SURGE_PROJECTILES, Player>::invoke_render();
    };

    IngameDialogueBox::instantiate(config::components::dialogue_box::initializer);

    Player::instantiate(SDL_Point{});   // This is required for below instantiations
    IngameMapHandler::instantiate(config::interface::levelName);
    IngameViewHandler::instantiate(renderIngameDependencies, Player::instance->mDestRect);
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
    
    if (save.mPL.has_value()) {
        auto dataPL = new level::Data_Generic(save.mPL.value());
        level::data.erase(config::entities::player::typeID);
        level::data.insert(config::entities::player::typeID, dataPL);
    }

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    auto dataPL = level::data.get(config::entities::player::typeID);
    if (!dataPL.empty() && dataPL.front() != nullptr) Player::invoke(&Player::onLevelChange, *dataPL.front());

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
            if (Player::instance->mAnimation != Animation::kDeath) save.savetofile();   // Save progress
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
            Player::instance->mAttributes.heal();
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
        IMPL(level::Name::kLevelWoodsLongLane)
        IMPL(level::Name::kLevelWoodsCrossroadsFirst)
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
    // globals::currentLevelData.dataPL.destCoords = data.targetDestCoords;   // This has no effect if placed here
    save.mPL = std::make_optional<SDL_Point>(data.targetDestCoords);   // Cache

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

template <bool BLOCKING>
bool IngameInterface::isPlayerWithinRange(std::pair<int, int> const& x_lim, std::pair<int, int> const& y_lim) const {
    bool result = Player::instance->isWithinRange(x_lim, y_lim);

    if (BLOCKING && result) {
        Player::invoke(&Player::onMoveEnd, BehaviouralType::kInvalidated);
        Player::invoke(&Player::onRunningToggled, false);
    }

    return result;
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelWoodsLongLane>
IngameInterface::handleLevelSpecifics_impl() const {
    static constexpr std::array<int, 5> verticalCheckpoints = { 42, 38, 34, 30, 26, };
    static constexpr int verticalCheckpointFinal = 20;

    switch (mProgress.get()) {
        case 0:
            if (isPlayerWithinRange({ -1, -1 }, { verticalCheckpoints[1] + 1, verticalCheckpoints[0] })) {
                OmoriCat_0::invoke(&OmoriCat_0::syncPlayerMovement<false, true>);
                OmoriCat_1::invoke(&OmoriCat_1::syncPlayerMovement<false, true>);
                OmoriCat_2::invoke(&OmoriCat_2::syncPlayerMovement<false, true>);
                OmoriCat_3::invoke(&OmoriCat_3::syncPlayerMovement<false, true>);
                OmoriCat_4::invoke(&OmoriCat_4::syncPlayerMovement<false, true>);
                OmoriCat_5::invoke(&OmoriCat_5::syncPlayerMovement<false, true>);
                OmoriCat_6::invoke(&OmoriCat_6::syncPlayerMovement<false, true>);
                OmoriCat_7::invoke(&OmoriCat_7::syncPlayerMovement<false, true>);
            } else if (isPlayerWithinRange({ -1, -1 }, { verticalCheckpoints[2] + 1, verticalCheckpoints[1] })) {
                OmoriCat_1::invoke(&OmoriCat_1::syncPlayerMovement<false, true>);
                OmoriCat_2::invoke(&OmoriCat_2::syncPlayerMovement<false, true>);
                OmoriCat_3::invoke(&OmoriCat_3::syncPlayerMovement<false, true>);
                OmoriCat_4::invoke(&OmoriCat_4::syncPlayerMovement<false, true>);
                OmoriCat_5::invoke(&OmoriCat_5::syncPlayerMovement<false, true>);
                OmoriCat_6::invoke(&OmoriCat_6::syncPlayerMovement<false, true>);        
            } else if (isPlayerWithinRange({ -1, -1 }, { verticalCheckpoints[3] + 1, verticalCheckpoints[2] })) {
                OmoriCat_2::invoke(&OmoriCat_2::syncPlayerMovement<false, true>);
                OmoriCat_3::invoke(&OmoriCat_3::syncPlayerMovement<false, true>);
                OmoriCat_4::invoke(&OmoriCat_4::syncPlayerMovement<false, true>);
                OmoriCat_5::invoke(&OmoriCat_5::syncPlayerMovement<false, true>);
            } else if (isPlayerWithinRange({ -1, -1 }, { verticalCheckpoints[4] + 1, verticalCheckpoints[3] })) {
                OmoriCat_3::invoke(&OmoriCat_3::syncPlayerMovement<false, true>);
                OmoriCat_4::invoke(&OmoriCat_4::syncPlayerMovement<false, true>);
            }

            if (isPlayerWithinRange({ -1, -1 }, { -1, verticalCheckpointFinal })) mProgress.increment();
            break;

        case 1:
            for (auto& instance : OmoriCat_0::instances) instance->mDestCoords.y = verticalCheckpoints[1];
            for (auto& instance : OmoriCat_1::instances) instance->mDestCoords.y = verticalCheckpoints[2];
            for (auto& instance : OmoriCat_2::instances) instance->mDestCoords.y = verticalCheckpoints[3];
            for (auto& instance : OmoriCat_3::instances) instance->mDestCoords.y = verticalCheckpoints[4];
            for (auto& instance : OmoriCat_4::instances) instance->mDestCoords.y = verticalCheckpoints[4];
            for (auto& instance : OmoriCat_5::instances) instance->mDestCoords.y = verticalCheckpoints[3];
            for (auto& instance : OmoriCat_6::instances) instance->mDestCoords.y = verticalCheckpoints[2];
            for (auto& instance : OmoriCat_7::instances) instance->mDestCoords.y = verticalCheckpoints[1];
            Invoker<INTERACTABLES>::invoke_onWindowChange();

            mProgress.increment();
            break;

        default:
            for (const auto& instance : OmoriCat_4::instances) if (instance->mDestCoords.y != verticalCheckpoints[4]) mProgress.set(1);
    }
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelWoodsCrossroadsFirst>
IngameInterface::handleLevelSpecifics_impl() const {
    if (isPlayerWithinRange<true>({ -1, 2 }, { -1, -1 })) IngameDialogueBox::invoke(&IngameDialogueBox::enqueueContents, std::vector<std::string>({
        ".You had failed them, remember? You were unworthy.",
    }));
}

template <level::Name L>
typename std::enable_if_t<L == level::Name::kLevelWhiteSpace>
IngameInterface::handleLevelSpecifics_impl() const {
    switch (mProgress.get()) {
        case 0:   // Border is not traversed
            break;

        case 1:   // Border is traversed at least once
            if (!RedHandThrone::instances.empty()) break;
            RedHandThrone::instantiateEX({
                new level::Data_Teleporter{ { 52, 43 }, { 9, 9 }, level::Name::kLevelPrelude },
            });
            break;

        default: break;
    }

    static auto infiniteLoopEffectHandler = [&](int& i, const double lower, const double upper) {
        auto callback = [&]() {
            if (mProgress.get() == 0) mProgress.increment();
            RedHandThrone::invoke(&RedHandThrone::onWindowChange);
        };

        if (i <= lower) {
            i = upper;
            callback();
        } else if (i >= upper) {
            i = lower;
            callback();
        }

        // return (i <= lower) ? upper : (i >= upper) ? lower : i;
    };   // Declaring as `static` would yield `warning: storing the address of local variable ‘isBorderTraversed’ in ‘kInternalTeleportHandler.IngameInterface::handleLevelSpecifics_kLevelWhiteSpace() const::<lambda(int&, double, double)>::<isBorderTraversed capture>’ [-Wdangling-pointer=]`

    // "Infinite loop" effect
    if (Player::instance->mNextDestCoords != nullptr) {
        infiniteLoopEffectHandler(Player::instance->mNextDestCoords->x, IngameViewHandler::instance->mTileCountWidth / 2 + 1, level::data.tileDestCount.x - IngameViewHandler::instance->mTileCountWidth / 2 - 1);
        infiniteLoopEffectHandler(Player::instance->mNextDestCoords->y, IngameViewHandler::instance->mTileCountHeight / 2 + 2, level::data.tileDestCount.y - IngameViewHandler::instance->mTileCountHeight / 2 - 1);   // Slight deviation to prevent "staggering"
    }
}