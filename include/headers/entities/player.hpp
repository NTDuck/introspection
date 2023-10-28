#pragma once

#include <SDL.h>
#include <meta.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief Represents the player.
 * @todo The player seems to be shifted to the left.
*/
class Player : public TextureWrapper {
    public:
        Player();
        ~Player();

        void init();
        void handleKeyboardEvent(const SDL_Event& event);
};