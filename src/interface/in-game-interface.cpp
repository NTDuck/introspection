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
    PentacleProjectile::invoke(&PentacleProjectile::handleLifespan);
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

// template <typename Active, typename Passive>
// void IngameInterface::onEntityCollision(Active& active, Passive& passive) const {
//     // ...
// }

// template <>
// void IngameInterface::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter) const {

// }

// template <>
// void IngameInterface::onEntityCollision<Player, Slime>(Player& player, Slime& slime) const {
//     // state = GameState::kExit;
//     // player.onDeath();
// }

// /**
//  * @brief Called when the `active` entity initiate an animation (possibly caused by the `passive` entity).
// */
// template <typename Active, typename Passive>
// void IngameInterface::onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) const {
//     // Handle `kDamaged` case differently
//     if (animationType == AnimationType::kDamaged && passive.currAnimationType == AnimationType::kAttack) {
//         active.secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(passive.secondaryStats, active.secondaryStats);
//         active.secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedMagicDamage(passive.secondaryStats, active.secondaryStats);
//         if (active.secondaryStats.HP <= 0) animationType = AnimationType::kDeath;
//     }

//     if ((active.nextAnimationType == nullptr) || (!active.isAnimationOnProgress&& !(*active.nextAnimationType == AnimationType::kDamaged && animationType == AnimationType::kAttack))) {
//         active.nextAnimationType = new AnimationType(animationType);
//         active.isAnimationOnProgress = false;
//     }
// }

// template void IngameInterface::onEntityAnimation<Player, Slime>(const AnimationType animationType, Player& player, Slime& slime) const;
// template void IngameInterface::onEntityAnimation<Slime, Player>(const AnimationType animationType, Slime& slime, Player& player) const;
// template void IngameInterface::onEntityAnimation<PentacleProjectile, Player>(const AnimationType animationType, PentacleProjectile& surgeAttackObject, Player& player) const;
// template void IngameInterface::onEntityAnimation<HauntedBookcaseProjectile, Player>(const AnimationType animationType, HauntedBookcaseProjectile& projectile, Player& player) const;

// /**
//  * @brief Handle interactions between entities.
// */
// void IngameInterface::handleEntitiesInteraction() const {
//     auto teleporter = utils::checkEntityCollision<Player, Teleporter>(*Player::instance, InteractionType::kCoordsArb); if (teleporter != nullptr) onEntityCollision<Player, Teleporter>(*Player::instance, *teleporter);
    // auto slime = utils::checkEntityCollision<Player, Slime>(*Player::instance, InteractionType::kRect); if (slime != nullptr) onEntityCollision<Player, Slime>(*Player::instance, *slime);

    // // Friendly fire shall not be tolerated!
    // for (auto& surgeAttackObject : SurgeAttackObject::instances) {
    //     if (surgeAttackObject == nullptr) continue;
    //     if (utils::checkEntityAttackRegister<Player, SurgeAttackObject>(*Player::instance, *surgeAttackObject, false)) onEntityAnimation<Player, SurgeAttackObject>(AnimationType::kDamaged, *Player::instance, *surgeAttackObject);
    // }

    // for (auto& slime : Slime::instances) {
    //     if (slime == nullptr || slime->currAnimationType == AnimationType::kDeath) continue;
    //     if (utils::checkEntityAttackInitiate<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kAttack, *slime, *Player::instance);
    //     // if (utils::checkEntityAttackRegister<Player, Slime>(*Player::instance, *slime)) onEntityAnimation<Player, Slime>(AnimationType::kDamaged, *Player::instance, *slime);
    //     // if (utils::checkEntityAttackRegister<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kDamaged, *slime, *Player::instance);

    //     for (auto& surgeAttackObject : PentacleProjectile::instances) {
    //         if (surgeAttackObject == nullptr) continue;
    //         // if (utils::checkEntityAttackRegister<Slime, PentacleProjectile>(*slime, *surgeAttackObject, false)) onEntityAnimation<Slime, PentacleProjectile>(AnimationType::kDamaged, *slime, *surgeAttackObject);
    //     }

    //     for (auto& projectile : HauntedBookcaseProjectile::instances) {
    //         if (projectile == nullptr) continue;
    //         // if (utils::checkEntityAttackRegister<Slime, HauntedBookcaseProjectile>(*slime, *projectile, false)) onEntityAnimation<Slime, HauntedBookcaseProjectile>(AnimationType::kDamaged, *slime, *projectile);
    //     }
    // }

//     if (Player::instance->currAnimationType == AnimationType::kDeath) {
//         IngameMapHandler::instance->isOnGrayscale = true;
//         if (Player::invoke(&Player::isAnimationAtFinalSprite)) globals::state = GameState::kGameOver;
//     }
// }

void IngameInterface::handleEntitiesSFX() const {
    Player::invoke(&Player::handleSFX);
    // Teleporter::invoke(&Teleporter::handleSFX);
    Slime::invoke(&Slime::handleSFX);
}

void IngameInterface::handleCustomEventGET_kResp_Teleport_GTE_Player(SDL_Event const& event) const {
    auto data = *reinterpret_cast<event::data::kReq_Teleport_GTE_Player*>(event.user.data1);
    
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