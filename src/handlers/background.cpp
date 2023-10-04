#include <iostream>
#include <string>

#include <handlers/background.h>

#include <auxiliaries/defs.h>
#include <auxiliaries/structs.h>
#include <utils/sdl2.h>


BackgroundHandler::BackgroundHandler(Dimensions dimensions) {
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = dimensions._w;
    stretchRect.h = dimensions._h;

    position = static_cast<unsigned char>(BackgroundType::curr);
}

BackgroundHandler::~BackgroundHandler() {
    if (background != nullptr) SDL_FreeSurface(background);
}

void BackgroundHandler::changeBackground(SDL_Surface* windowSurface, BackgroundType type, SDL_PixelFormat* fmt) {
    std::string path;

    switch (type) {
        case BackgroundType::prev:
            path = "assets/images/backgrounds/astley.png"; break;
        case BackgroundType::curr:
            path = "assets/images/backgrounds/lowres.png"; break;
        case BackgroundType::next:
            path = "assets/images/backgrounds/konata.png"; break;
    }

    SDL_Surface* background = utils::optimize(path, fmt);
    SDL_BlitScaled(background, NULL, windowSurface, &stretchRect);
    delete background;
}