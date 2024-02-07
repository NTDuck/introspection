#include <auxiliaries.hpp>

#include <SDL.h>


uint32_t event::type = -1;

void event::initialize() {
    if (event::type != (uint32_t)-1) return;
    event::type = SDL_RegisterEvents(1);
}

SDL_Event event::instantiate() {
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event));

    event.type = event::type;
    event.user.data1 = event.user.data2 = nullptr;

    return event;
}

/**
 * @brief Deallocate `event.user.data1` and `event.user.data2`.
 * @note Assume `event.user.data1` type matches `event.user.code`.
 * @warning Might be prone to memory leaks.
*/
void event::terminate(SDL_Event const& event) {
    // Deleting `void*` is undefined
    switch (event::getCode(event)) {
        case event::Code::kReq_DeathPending_Player:
        case event::Code::kReq_DeathFinalized_Player:
            event::__deallocate__<event::ID>(event);
            break;

        case event::Code::kReq_AttackRegister_Player_GHE:
        case event::Code::kReq_AttackRegister_GHE_Player:
        case event::Code::kReq_AttackInitiate_GHE_Player:
        case event::Code::kReq_MoveInitiate_GHE_Player:
        case event::Code::kResp_AttackInitiate_GHE_Player:
        case event::Code::kResp_MoveInitiate_GHE_Player:
        case event::Code::kResp_MoveTerminate_GHE_Player:
            event::__deallocate__<event::data::Mob>(event);
            break;

        case event::Code::kReq_Teleport_GTE_Player:
        case event::Code::kResp_Teleport_GTE_Player:
            event::__deallocate__<event::data::Teleporter>(event);
            break;
        
        default: break;
    }

    if (event.user.data2 != nullptr) delete reinterpret_cast<event::ID*>(event.user.data2);
}

void event::enqueue(SDL_Event& event) {
    if (event.user.data1 != nullptr) SDL_PushEvent(&event);
    else delete reinterpret_cast<event::ID*>(event.user.data2);
}

event::ID event::getID(SDL_Event const& event) {
    return *reinterpret_cast<event::ID*>(event.user.data2);
}

void event::setID(SDL_Event& event, int id) {
    if (event.user.data2 != nullptr) delete reinterpret_cast<event::ID*>(event.user.data2);
    event.user.data2 = new event::ID(id);
}

event::Code event::getCode(SDL_Event const& event) {
    return static_cast<event::Code>(event.user.code);
}

void event::setCode(SDL_Event& event, event::Code code) {
    event.user.code = static_cast<Sint32>(code);
}