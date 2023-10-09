#include <iostream>
#include <array>

#include <SDL.h>
#include <SDL_image.h>

#include <wrappers/texture.h>
#include <characters/player.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <utils/sdl2.h>


Player::Player(Dat2u coords) {
    destRect.x = coords._x;
    destRect.y = coords._y;
}

Player::~Player() {}

void Player::init(SDL_Window* window, SDL_Renderer* renderer) {
    limRect.x = 0;
    limRect.y = 0;
    SDL_GetWindowSize(window, &limRect.w, &limRect.h);
    
    loadSpriteSheet(renderer);
}

/**
 * @brief Handle keyboard events.
*/
void Player::handleKeyboardEvent(const SDL_Event* event) {
    if (event -> key.repeat != 0) return;

    switch (event -> key.keysym.sym) {
        case SDLK_w:
            if (event -> type == SDL_KEYDOWN) {
                vel._y -= VEL;
                TextureWrapper::setPriorityState(SpriteState::WALKING);
            } else {
                vel._y += VEL;
                state = SpriteState::IDLE;
            } break;

        case SDLK_s:
            if (event -> type == SDL_KEYDOWN) {
                vel._y += VEL;
                TextureWrapper::setPriorityState(SpriteState::WALKING);
            } else {
                vel._y -= VEL;
                state = SpriteState::IDLE;
            } break;

        case SDLK_a:
            flip = SDL_FLIP_HORIZONTAL;
            if (event -> type == SDL_KEYDOWN) {
                vel._x -= VEL;
                TextureWrapper::setPriorityState(SpriteState::WALKING);
            } else {
                vel._x += VEL;
                state = SpriteState::IDLE;
            } break;

        case SDLK_d:
            flip = SDL_FLIP_NONE;
            if (event -> type == SDL_KEYDOWN) {
                vel._x += VEL;
                TextureWrapper::setPriorityState(SpriteState::WALKING);
            } else {
                vel._x -= VEL;
                state = SpriteState::IDLE;
            } break;
    }
}

/**
 * @see https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent
*/
void Player::handleMouseEvent(const SDL_Event* event) {
    switch (event -> type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event -> button.button == SDL_BUTTON_LEFT) {
                setPriorityState(SpriteState::ATTACK_0);
            } else {
                setPriorityState(SpriteState::SHIELDING);
            } break;
    }
}

/**
 * @brief Prepare the spritesheets for rendering.
 * @param renderer usually the global renderer. Required for loading textures.
*/
void Player::loadSpriteSheet(SDL_Renderer* renderer) {
    std::string path_0 = "assets/images/sprites/player/generic/generic-full-0-x2.56.png";
    SDL_Texture* texture_0 = IMG_LoadTexture(renderer, path_0.c_str());
    Dat2u sheetSize_0;
    SDL_QueryTexture(texture_0, NULL, NULL, &sheetSize_0._x, &sheetSize_0._y);
    Dat2hu sheetDivs_0 = {8, 11};

    std::string path_1 = "assets/images/sprites/player/generic/generic-full-1-x2.56.png";
    SDL_Texture* texture_1 = IMG_LoadTexture(renderer, path_1.c_str());
    Dat2u sheetSize_1;
    SDL_QueryTexture(texture_1, NULL, NULL, &sheetSize_1._x, &sheetSize_1._y);
    Dat2hu sheetDivs_1 = {8, 7};

    TextureWrapper::loadSpriteSheet({
        {SpriteState::IDLE, texture_0, sheetSize_0, sheetDivs_0, {0, 0}, 6},
        {SpriteState::ATTACK_0, texture_0, sheetSize_0, sheetDivs_0, {1, 0}, 6},
        {SpriteState::RUNNING, texture_0, sheetSize_0, sheetDivs_0, {2, 0}, 8},
        {SpriteState::DAMAGED, texture_0, sheetSize_0, sheetDivs_0, {5, 0}, 4},
        {SpriteState::DEATH, texture_0, sheetSize_0, sheetDivs_0, {6, 0}, 12},
        {SpriteState::ABILITY_0, texture_0, sheetSize_0, sheetDivs_0, {8, 0}, 8},
        {SpriteState::SHIELDING, texture_0, sheetSize_0, sheetDivs_0, {10, 1}, 2},
        {SpriteState::WALKING, texture_1, sheetSize_1, sheetDivs_1, {0, 0}, 10},
        {SpriteState::ATTACK_CRITICAL, texture_1, sheetSize_1, sheetDivs_1, {4, 0}, 8},
    });

    destRect.w = sheetSize_0._x / sheetDivs_0._x;
    destRect.h = sheetSize_0._y / sheetDivs_0._y;
}

void Player::move() {
    destRect.x += vel._x;
    destRect.y += vel._y;

    // std::cout << destRect.x << " " << destRect.y << " " << destRect.w << " " << destRect.h << std::endl;

    // adjust offset if out of bound
    if (destRect.x < limRect.x || destRect.x + destRect.w > limRect.w) destRect.x -= vel._x;
    if (destRect.y < limRect.y || destRect.y + destRect.h > limRect.h) destRect.y -= vel._y;
}