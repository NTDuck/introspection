#pragma once

#include <string>
#include <unordered_map>

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

        void render() override;
        void move() override;

    private:
        void updateAnimation();
        void resetAnimation(const std::string nextAnimationState);

        void onMoveStart();
        void onMoveEnd() override;

        /**
         * @brief Maps the animation state with its respective starting and stopping GIDs.
         * @todo Consider implementing `enum` instead of `std::string`.
         * @todo Optimize retrieval.
        */
        std::unordered_map<std::string, std::pair<int, int>> rotatingGIDs;
        std::string currAnimationState;
        int currAnimationGID;

        /**
         * @brief The maximum number of tiles per dimension in the tileset.
        */
        SDL_Point srcRectCount;

        /**
         * @brief Indicates the number of frames a sprite should last before switching to the next. Should be treated as a constant.
        */
        int animationUpdateRate;
};