#ifndef INTERACTION_H
#define INTERACTION_H

#include <auxiliaries/globals.hpp>


namespace utils {
    template <class Active, class Passive>
    Passive* checkCollision(const Active& active, InteractionType interactionType);

    template <class Active, class Passive>
    bool checkAttack(const Active& active, const Passive& passive);

    template <class Active, class Passive>
    bool checkDamaged(const Active& active, const Passive& passive);
};


#endif