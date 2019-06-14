#include "Game.h"

Game::Game() {
    
}

Game::~Game() {
    delete Camera;
    delete Input;
}

void Game::Initialize() {
    Camera = new Camera();
    Input = new InputManager();
}