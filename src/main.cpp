#include <fstream>
#include <string>

#include <SDL.h>

#include <game.h>
#include <auxiliaries/globals.h>


int main(int argc, char* args[]) {
    Flags flags; SDL_Rect dims; int frameRate; std::string title = "8964";
    utils::parseConfigData("config.json", flags, dims, frameRate);

    Game game(flags, dims, frameRate, title);
    game.start();

    return 0;
}