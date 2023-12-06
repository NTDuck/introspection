#include <interaction.hpp>

#include <algorithm>
#include <functional>
#include <unordered_map>

#include <entities.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


template <class Active, class Passive>
bool predicateCoordsCollision(const Active& active, const Passive& passive) {
    return passive.destCoords.x == active.nextDestCoords->x && passive.destCoords.y == active.nextDestCoords->y;
}

template <class Active, class Passive>
bool predicateRectCollision(const Active& active, const Passive& passive) {
    return (active.destRect.x < passive.destRect.x ? passive.destRect.x < active.destRect.x + active.destRect.w : active.destRect.x < passive.destRect.x + passive.destRect.w) && (active.destRect.y < passive.destRect.y ? passive.destRect.y < active.destRect.y + active.destRect.h : active.destRect.y < passive.destRect.y + passive.destRect.h);   // Standard bounding box collision detection
}

/**
 * @brief Check for collision between the entity `active` and an entity of class `Passive`.
 * @note Assume that no more than one entity of class `Passive` could collide with the entity `active` simutaneously.
 * @note Please do attempt to fix this mess.
*/
template <class Active, class Passive>
Passive* utils::checkCollision(const Active& active, InteractionType interactionType) {
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
 * @brief Check whether the `active` entity is able to initiate an attack onto the `passive` entity.
*/
template <class Active, class Passive>
bool utils::checkAttack(const Active& active, const Passive& passive) {
    if (
        active.currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kAttack
        || passive.currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged
    ) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    if (distance > active.kAttackInitiateRange.x || distance > active.kAttackInitiateRange.y) return false;

    return true;
}

/**
 * @brief Check whether the `active` entity is able to be damaged by the `passive` entity.
*/
template <class Active, class Passive>
bool utils::checkDamaged(const Active& active, const Passive& passive) {
    if (
        active.currAnimationType == tile::AnimatedEntitiesTilesetData::AnimationType::kDamaged
        || passive.currAnimationType != tile::AnimatedEntitiesTilesetData::AnimationType::kAttack
        || !passive.isAnimationAtFinalSprite
    ) return false;

    int distance = utils::calculateDistance(active.destCoords, passive.destCoords);
    if (distance > passive.kAttackRegisterRange.x || distance > passive.kAttackRegisterRange.y) return false;

    return true;
}


template Teleporter* utils::checkCollision<Player, Teleporter>(const Player& player, InteractionType interactionType);

template Slime* utils::checkCollision<Player, Slime>(const Player& player, InteractionType interactionType);


template bool utils::checkAttack<Slime, Player>(const Slime& slime, const Player& player);
template bool utils::checkDamaged<Player, Slime>(const Player& player, const Slime& slime);
template bool utils::checkDamaged<Slime, Player>(const Slime& slime, const Player& player);