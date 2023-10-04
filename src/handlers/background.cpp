#include <iostream>
#include <string>

#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>


BackgroundHandler::BackgroundHandler(Dimensions dimensions) {
    position = static_cast<unsigned char>(BackgroundType::curr);
}

BackgroundHandler::~BackgroundHandler() {
    if (background != nullptr) SDL_DestroyTexture(background);
}

void BackgroundHandler::changeBackground(SDL_Renderer* renderer, BackgroundType type) {
    std::string path;

    switch (type) {
        case BackgroundType::prev:
            path = "assets/images/backgrounds/astley.png"; break;
        case BackgroundType::curr:
            path = "assets/images/backgrounds/lowres.png"; break;
        case BackgroundType::next:
            path = "assets/images/backgrounds/konata.png"; break;
    }

    // background = IMG_LoadTexture(renderer, path.c_str());
    background = IMG_LoadTexture(renderer, path.c_str());
    // delete background;
}