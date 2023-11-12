#include <game.hpp>
#include <auxiliaries/globals.hpp>


int main(int argc, char* args[]) {
    Game game(globals::config::flags, globals::config::dims, globals::config::frameRate, "8964");
    game.start();

    return 0;
}