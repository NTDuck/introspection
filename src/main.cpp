#include <game.hpp>
#include <auxiliaries/globals.hpp>


int main(int argc, char* args[]) {
    Game game(config::flags, config::dims, config::frameRate, "8964");
    game.start();

    return 0;
}