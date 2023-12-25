#include <interaction.hpp>

#include <algorithm>
#include <functional>
#include <unordered_map>

#include <entities.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


template <typename Active, typename Passive>
bool predicateCoordsCollision(const Active& active, const Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedDynamicEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractEntity<Passive>, Passive>();

    return passive.destCoords == *active.nextDestCoords;
}

template <typename Active, typename Passive>
bool predicateRectCollision(const Active& active, const Passive& passive) {
    utils::isDerivedFrom<AbstractEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractEntity<Passive>, Passive>();

    return (active.destRect.x < passive.destRect.x ? passive.destRect.x < active.destRect.x + active.destRect.w : active.destRect.x < passive.destRect.x + passive.destRect.w) && (active.destRect.y < passive.destRect.y ? passive.destRect.y < active.destRect.y + active.destRect.h : active.destRect.y < passive.destRect.y + passive.destRect.h);   // Standard bounding box collision detection
}

/**
 * @brief Check for collision between the entity `active` and an entity of typename `Passive`.
 * @note Assume that no more than one entity of typename `Passive` could collide with the entity `active` simutaneously.
 * @note Please do attempt to fix this mess.
*/
template <typename Active, typename Passive>
Passive* utils::checkEntityCollision(const Active& active, InteractionType interactionType) {
    static const std::unordered_map<InteractionType, std::function<bool(const Active&, Passive&)>> mapping = {
        {InteractionType::kCoords, &predicateCoordsCollision<Active, Passive>},
        {InteractionType::kRect, &predicateRectCollision<Active, Passive>},
    };
    auto predicate = mapping.find(interactionType);
    if (predicate == mapping.end()) return nullptr;   // Throw

    static auto it = Passive::instances.end();
    if (interactionType != InteractionType::kCoords || active.nextDestCoords != nullptr) it = std::find_if(
        Passive::instances.begin(), Passive::instances.end(),
        [&](const auto& instance) { return predicate->second(active, *instance); }
    );

    if (it == Passive::instances.end() || (interactionType == InteractionType::kCoords && active.nextDestCoords != nullptr)) return nullptr;
    
    Passive* target = *it;
    it = Passive::instances.end();   // Without this the program magically terminates
    return target;
}

/**
 * @brief Check whether the `active` entity is currently able to initiate an attack onto the `passive` entity.
*/
template <typename Active, typename Passive>
bool utils::checkEntityAttackInitiate(const Active& active, const Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();

    if (active.currAnimationType == AnimationType::kAttack || passive.currAnimationType == AnimationType::kDamaged) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    return !(distance > active.kAttackInitiateRange.x || distance > active.kAttackInitiateRange.y);
}

/**
 * @brief Check whether the `active` entity is currently able to be damaged by the `passive` entity.
*/
template <typename Active, typename Passive>
bool utils::checkEntityAttackRegister(const Active& active, const Passive& passive) {
    utils::isDerivedFrom<AbstractAnimatedEntity<Active>, Active>();
    utils::isDerivedFrom<AbstractAnimatedEntity<Passive>, Passive>();
    
    if (active.currAnimationType == AnimationType::kDamaged || passive.currAnimationType != AnimationType::kAttack || !passive.isAnimationAtFinalSprite) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    return !(distance > passive.kAttackRegisterRange.x || distance > passive.kAttackRegisterRange.y);
}


template Teleporter* utils::checkEntityCollision<Player, Teleporter>(const Player& player, InteractionType interactionType);
template Slime* utils::checkEntityCollision<Player, Slime>(const Player& player, InteractionType interactionType);
template bool utils::checkEntityAttackInitiate<Slime, Player>(const Slime& slime, const Player& player);
template bool utils::checkEntityAttackRegister<Player, Slime>(const Player& player, const Slime& slime);
template bool utils::checkEntityAttackRegister<Slime, Player>(const Slime& slime, const Player& player);