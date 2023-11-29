#ifndef INTERACTION_H
#define INTERACTION_H

#include <auxiliaries/globals.hpp>


namespace utils {
    template <class Active, class Passive>
    Passive* checkCollision(const Active& active, InteractionType interactionType);
};


#endif