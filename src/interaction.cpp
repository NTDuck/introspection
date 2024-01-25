#include <interaction.hpp>

#include <algorithm>
#include <functional>
#include <unordered_map>

#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @note Should only be used for Player - Teleporter collision.
*/
template <typename Active, typename Passive>
bool predicateCoordsArbCollision(Active const& active, Passive const& passive) {
    return passive.destCoords == *active.nextDestCoords;
}

template <typename Active, typename Passive>
bool predicateCoordsAbsCollision(Active const& active, Passive const& passive) {
    return passive.destCoords == active.destCoords || passive.destCoords == *active.nextDestCoords;
}

template <typename Active, typename Passive>
bool predicateRectCollision(Active const& active, Passive const& passive) {
    return (active.destRect.x < passive.destRect.x ? passive.destRect.x < active.destRect.x + active.destRect.w : active.destRect.x < passive.destRect.x + passive.destRect.w) && (active.destRect.y < passive.destRect.y ? passive.destRect.y < active.destRect.y + active.destRect.h : active.destRect.y < passive.destRect.y + passive.destRect.h);   // Standard bounding box collision detection
}

/**
 * @brief Check for collision between the entity `active` and an entity of typename `Passive`.
 * @note Assume that no more than one entity of typename `Passive` could collide with the entity `active` simutaneously.
 * @note Please do attempt to fix this mess.
*/
template <typename Active, typename Passive>
Passive* utils::checkEntityCollision(Active const& active, InteractionType interactionType) {
    static const std::unordered_map<InteractionType, std::function<bool(Active const&, Passive&)>> mapping = {
        { InteractionType::kCoordsArb, &predicateCoordsArbCollision<Active, Passive> },
        { InteractionType::kCoordsAbs, &predicateCoordsAbsCollision<Active, Passive> },
        { InteractionType::kRect, &predicateRectCollision<Active, Passive> },
    };
    auto predicate = mapping.find(interactionType);
    if (predicate == mapping.end()) return nullptr;   // Throw

    static auto it = Passive::instances.end();
    if (interactionType != InteractionType::kCoordsArb || active.nextDestCoords != nullptr) it = std::find_if(
        Passive::instances.begin(), Passive::instances.end(),
        [&](const auto& instance) { return predicate->second(active, *instance); }
    );

    if (it == Passive::instances.end() || (interactionType == InteractionType::kCoordsArb && active.nextDestCoords != nullptr)) return nullptr;
    
    Passive* target = *it;
    it = Passive::instances.end();   // Without this the program magically terminates
    return target;
}

/**
 * @brief Check whether the `active` entity is currently able to initiate an attack onto the `passive` entity.
*/
template <typename Active, typename Passive>
bool utils::checkEntityAttackInitiate(Active const& active, Passive const& passive) {
    if (active.currAnimationType == AnimationType::kAttack || passive.currAnimationType == AnimationType::kDamaged) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    return !(distance > active.kAttackInitiateRange.x || distance > active.kAttackInitiateRange.y);
}

/**
 * @brief Check whether the `active` entity is currently able to be damaged by the `passive` entity.
*/
template <typename Active, typename Passive>
bool utils::checkEntityAttackRegister(Active const& active, Passive const& passive, bool requiresPassiveAtFinalSprite) {
    if (active.currAnimationType == AnimationType::kDamaged || passive.currAnimationType != AnimationType::kAttack || (requiresPassiveAtFinalSprite && !passive.isAnimationAtFinalSprite())) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    return !(distance > passive.kAttackRegisterRange.x || distance > passive.kAttackRegisterRange.y);
}


template Teleporter* utils::checkEntityCollision<Player, Teleporter>(Player const& player, InteractionType interactionType);
template Slime* utils::checkEntityCollision<Player, Slime>(Player const& player, InteractionType interactionType);
template Player* utils::checkEntityCollision<SurgeAttackObject, Player>(SurgeAttackObject const& surgeAttackObject, InteractionType interactionType);
template Slime* utils::checkEntityCollision<SurgeAttackObject, Slime>(SurgeAttackObject const& surgeAttackObject, InteractionType interactionType);

template bool utils::checkEntityAttackInitiate<Slime, Player>(Slime const& slime, Player const& player);
template bool utils::checkEntityAttackRegister<Player, Slime>(Player const& player, Slime const& slime, bool requiresPassiveAtFinalSprite);
template bool utils::checkEntityAttackRegister<Slime, Player>(Slime const& slime, Player const& player, bool requiresPassiveAtFinalSprite);
template bool utils::checkEntityAttackRegister<SurgeAttackObject, Player>(SurgeAttackObject const& surgeAttackObject, Player const& player, bool requiresPassiveAtFinalSprite);
template bool utils::checkEntityAttackRegister<SurgeAttackObject, Slime>(SurgeAttackObject const& surgeAttackObject, Slime const& slime, bool requiresPassiveAtFinalSprite);