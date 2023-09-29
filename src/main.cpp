#define SDL_MAIN_HANDLED   // fix undefined reference to `WinMain@16'
#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "it works!" << std::endl;
    std::cin.get();   // prevent terminal exit
    return 0;
}