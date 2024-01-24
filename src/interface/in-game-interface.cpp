#include <interface.hpp>

#include <interaction.hpp>
#include <entities.hpp>
#include <mixer.hpp>
#include <auxiliaries.hpp>


IngameInterface::IngameInterface() {
    auto renderIngameDependencies = []() {
        IngameMapHandler::invoke(&IngameMapHandler::render);
        Teleporter::invoke(&Teleporter::render);
        Slime::invoke(&Slime::render);
        Player::invoke(&Player::render);
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

    Player::deinitialize();
    Teleporter::deinitialize();
    Slime::deinitialize();
}

void IngameInterface::initialize() {
    IngameMapHandler::initialize();

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

    // Make changes to dependencies based on populated `globals::currentLevelData` members
    Player::invoke(&Player::onLevelChange, globals::currentLevelData.playerLevelData);
    Teleporter::onLevelChangeAll<level::TeleporterLevelData>(globals::currentLevelData.teleportersLevelData);
    Slime::onLevelChangeAll<level::SlimeLevelData>(globals::currentLevelData.slimesLevelData);

    Mixer::invoke(&Mixer::onLevelChange, IngameMapHandler::instance->getLevel());   // `IngameMapHandler::invoke(&IngameMapHandler::getLevel))` is not usable since the compiler cannot deduce "incomplete" type
}

void IngameInterface::onWindowChange() const {
    IngameMapHandler::invoke(&IngameMapHandler::onWindowChange);
    IngameViewHandler::invoke(&IngameViewHandler::onWindowChange);

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

void IngameInterface::handleEntities() const {
    handleEntitiesMovement();
    handleEntitiesInteraction();
}

/**
 * @brief Handle all entities movements & animation updates.
*/
void IngameInterface::handleEntitiesMovement() const {
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
void IngameInterface::onEntityCollision(Active& active, Passive& passive) const {
    // ...
}

template <>
void IngameInterface::onEntityCollision<Player, Teleporter>(Player& player, Teleporter& teleporter) const {
    IngameMapHandler::invoke(&IngameMapHandler::changeLevel, teleporter.targetLevel);
    globals::currentLevelData.playerLevelData.destCoords = teleporter.targetDestCoords;

    // globals::state = GameState::kLoading | GameState::kIngamePlaying;
    onLevelChange();
    onWindowChange();
}

template <>
void IngameInterface::onEntityCollision<Player, Slime>(Player& player, Slime& slime) const {
    // state = GameState::kExit;
    // player.onDeath();
}

/**
 * @brief Called when the `active` entity initiate an animation (possibly caused by the `passive` entity).
*/
template <typename Active, typename Passive>
void IngameInterface::onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) const {
    // Handle `kDamaged` case differently
    if (animationType == AnimationType::kDamaged && passive.currAnimationType == AnimationType::kAttack) {
        active.secondaryStats.HP -= EntitySecondaryStats::calculateFinalizedPhysicalDamage(passive.secondaryStats, active.secondaryStats);
        if (active.secondaryStats.HP <= 0) animationType = AnimationType::kDeath;
    }

    if ((active.nextAnimationType == nullptr) || (!active.isAnimationOnProgress&& !(*active.nextAnimationType == AnimationType::kDamaged && animationType == AnimationType::kAttack))) active.nextAnimationType = new AnimationType(animationType);
}

template void IngameInterface::onEntityAnimation<Player, Slime>(const AnimationType animationType, Player& player, Slime& slime) const;
template void IngameInterface::onEntityAnimation<Slime, Player>(const AnimationType animationType, Slime& slime, Player& player) const;

/**
 * @brief Handle interactions between entities.
*/
void IngameInterface::handleEntitiesInteraction() const {
    auto teleporter = utils::checkEntityCollision<Player, Teleporter>(*Player::instance, InteractionType::kCoords); if (teleporter != nullptr) onEntityCollision<Player, Teleporter>(*Player::instance, *teleporter);
    auto slime = utils::checkEntityCollision<Player, Slime>(*Player::instance, InteractionType::kRect); if (slime != nullptr) onEntityCollision<Player, Slime>(*Player::instance, *slime);

    for (auto& slime : Slime::instances) {
        if (slime == nullptr || slime->currAnimationType == AnimationType::kDeath) continue;
        if (utils::checkEntityAttackInitiate<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kAttack, *slime, *Player::instance);
        if (utils::checkEntityAttackRegister<Player, Slime>(*Player::instance, *slime)) onEntityAnimation<Player, Slime>(AnimationType::kDamaged, *Player::instance, *slime);
        if (utils::checkEntityAttackRegister<Slime, Player>(*slime, *Player::instance)) onEntityAnimation<Slime, Player>(AnimationType::kDamaged, *slime, *Player::instance);
    }

    if (Player::instance->currAnimationType == AnimationType::kDeath && Player::instance->isAnimationAtFinalSprite) globals::state = GameState::kGameOver;
}