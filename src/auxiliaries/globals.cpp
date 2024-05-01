#include <auxiliaries.hpp>

#include <unordered_map>
#include <SDL.h>


SDL_Renderer* globals::renderer = nullptr;
SDL_Point globals::windowSize;
SDL_Point globals::mouseState;
GameState globals::state = GameState::kMenu;
GarbageCollector globals::gc;


/**
 * @brief Deallocate resources.
 * @note Should be called when the program terminates.
*/
void globals::deinitialize() {
    if (globals::renderer != nullptr) {
        SDL_DestroyRenderer(globals::renderer);
        globals::renderer = nullptr;
    }
}