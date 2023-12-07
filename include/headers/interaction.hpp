#ifndef INTERACTION_H
#define INTERACTION_H

#include <auxiliaries/globals.hpp>


namespace utils {
    template <class Active, class Passive>
    Passive* checkEntityCollision(const Active& active, InteractionType interactionType);

    template <class Active, class Passive>
    bool checkEntityAttackInitiate(const Active& active, const Passive& passive);

    template <class Active, class Passive>
    bool checkEntityAttackRegister(const Active& active, const Passive& passive);
};


#endif