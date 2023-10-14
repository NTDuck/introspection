// #pragma once

// #include <array>
// #include <map>
// #include <set>

// #include <SDL.h>
// #include <SDL_image.h>

// #include <auxiliaries/defs.h>
// #include <auxiliaries/structs.h>


// /**
//  * @brief A wrapper class to encapsulate methods and store information about the texture.
// */
// class TextureWrapper {
//     public:
//         TextureWrapper();
//         ~TextureWrapper();

//         void loadSpriteSheet(std::set<TextureWrapperInitData> sheetData);
//         void dealloc();

//         void nextSprite();
//         void storeState(SpriteState tempState);
//         void restoreState();
//         void setPriorityState(SpriteState newState);

//         void render(SDL_Renderer* renderer);

//         void setRGB(Uint8 r, Uint8 g, Uint8 b);
//         void setBlending(SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
//         void setAlpha(Uint8 alpha);
//         void setRGBA(SDL_Color col);

//         double angle = 0;
//         SDL_Point* center = NULL;
//         SDL_RendererFlip flip = SDL_FLIP_NONE;

//         SDL_Rect destRect;   // denotes current position and size

//         /**
//          * @note Controls animation speed. Setting this to `n` denotes that the sprite will update every `n` frames.
//         */
//         const unsigned char textureUpdateRate = 16;

//         /**
//          * @note A mapping that associates `SpriteState` with the respective vector of `SDL_Rect`, allowing different set of sprites to be rendered base on `spriteState`.
//          * @warning Requires a custom hash function.
//         */
//         std::map<SpriteState, TextureWrapperMappingData> spriteMapping;
// };