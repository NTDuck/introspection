// #pragma once

// #include <SDL.h>
// #include <SDL_image.h>

// #include <meta/texture.h>

// #include <auxiliaries/defs.h>
// #include <auxiliaries/structs.h>


// /**
//  * @brief A derived class from `TextureWrapper` that handles the player.
//  * @see <wrappers/texture.h> TextureWrapper (class)
// */
// class Player : public TextureWrapper {
//     public:
//         Player(Dat2u coords);
//         ~Player();

//         void init(SDL_Window* window, SDL_Renderer* renderer);
//         void handleKeyboardEvent(const SDL_Event* event);
//         void handleMouseEvent(const SDL_Event* event);

//         void loadSpriteSheet(SDL_Renderer* renderer);
//         void move();

//         SDL_Rect limRect;

//     private:        
//         Dat2u vel = {0, 0};
//         const unsigned char VEL = 2;
// };