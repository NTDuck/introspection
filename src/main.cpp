#include <string>

#include <SDL.h>

#include <game.h>
#include <auxiliaries/structs.h>


int main(int argc, char* args[]) {
    // configurations, will later be moved elsewhere
    Flags flags;
    flags.init = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    flags.window = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    flags.renderer = 0;
    flags.image = IMG_INIT_PNG;

    Dimensions dimensions;
    dimensions._x = SDL_WINDOWPOS_UNDEFINED;
    dimensions._y = SDL_WINDOWPOS_UNDEFINED;
    dimensions._w = 1280;
    dimensions._h = 720;

    unsigned short int frameRate = 120;
    std::string title = "8964";

    // actual instantiation
    Game game(flags, dimensions, frameRate, title);
    game.start();

    return 0;
}