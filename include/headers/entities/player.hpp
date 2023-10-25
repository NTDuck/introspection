#pragma once

#include <meta.hpp>


/**
 * @brief Represents the player.
 * @todo The player seems to be shifted to the left.
*/
class Player : public TextureWrapper {
    public:
        Player(SDL_Point destCoords);
        ~Player();

        void init();

        void handleKeyboardEvent(const SDL_Event& event, TileCollection& tileCollection);

    private:
        bool validateMove(SDL_Point nextDestCoords, const TileCollection& tileCollection);
};