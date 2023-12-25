#ifndef INTERACTION_H
#define INTERACTION_H

#include <auxiliaries.hpp>


namespace utils {
    template <typename Active, typename Passive>
    Passive* checkEntityCollision(const Active& active, InteractionType interactionType);

    template <typename Active, typename Passive>
    bool checkEntityAttackInitiate(const Active& active, const Passive& passive);

    template <typename Active, typename Passive>
    bool checkEntityAttackRegister(const Active& active, const Passive& passive);
};


#endif