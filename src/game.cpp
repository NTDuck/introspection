#include "../include/headers/game.h"


Game::Game() {
    window = nullptr;
    renderer = nullptr;
    screenWidth = 1280;
    screenHeight = 720;
    gamestate = GameState::PLAY;
};

Game::~Game() {
    SDL_Quit();
};

void Game::run() {
    init("8964", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    gameLoop();
}

void Game::init(const char* title, int _x, int _y, int _w, int _h, Uint32 flags) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title, _x, _y, _w, _h, flags);
    renderer = SDL_CreateRenderer(window, -1, 0);
}

void Game::gameLoop() {
    while (gamestate != GameState::EXIT) {
        handleEvents();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            gamestate = GameState::EXIT;
            break;
    }
}