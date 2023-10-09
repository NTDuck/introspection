#pragma once

/**
 * Represents all possible states of the game.
*/
enum GameState {MENU, PLAYING, PAUSED, CUTSCENE, EXIT};

/**
 * @brief Represents all possible states of a sprite.
*/
enum SpriteState {
    // "permanent" states
    IDLE, WALKING, RUNNING, CROUCHING, SHIELDING,
    // "temporary" states
    JUMPING, DODGING, ATTACK_0, ATTACK_1, ATTACK_2, ATTACK_CRITICAL, ABILITY_0, ABILITY_1, ABILITY_2, DAMAGED, DEATH,
};