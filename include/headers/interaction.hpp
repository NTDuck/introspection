#ifndef INTERACTION_H
#define INTERACTION_H

#include <auxiliaries.hpp>


namespace utils {
    template <typename Active, typename Passive>
    Passive* checkEntityCollision(Active const& active, InteractionType interactionType);

    template <typename Active, typename Passive>
    bool checkEntityAttackInitiate(Active const& active, Passive const& passive);

    template <typename Active, typename Passive>
    bool checkEntityAttackRegister(Active const& active, Passive const& passive, bool requiresPassiveAtFinalSprite = true);
};


#endif