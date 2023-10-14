#include <string>

#include <SDL.h>

#include <game.h>
#include <auxiliaries/defs.h>


int main(int argc, char* args[]) {
    // configurations, will later be moved elsewhere
    Flags flags;
    flags.init = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    flags.window = SDL_WINDOW_SHOWN;
    flags.renderer = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    flags.image = IMG_INIT_PNG;
    flags.hints[SDL_HINT_RENDER_SCALE_QUALITY] = "1";

    SDL_Rect dims {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720};

    const unsigned short int frameRate = 120;
    const std::string title = "8964";

    // actual instantiation
    Game game(flags, dims, frameRate, title);
    game.start();

    return 0;
}