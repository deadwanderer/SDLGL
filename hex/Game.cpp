#include "Game.h"

#include <iostream>

Game::Game() {
    std::cout << "Created game." << std::endl;
}

Game::~Game() {
    delete GameCamera;
    delete Input;
    delete GameClock;
    delete Grid;
}

void Game::Initialize() {
    GameCamera = new Camera(this);
    Input = new InputManager(this);
    Grid = new HexGrid(this);
    GameClock = new Clock(this);
    
    GameClock->Initialize();
    Grid->Initialize();
    
    running = true;
    shouldRender = false;
    std::cout << "Initialization complete." << std::endl;
}

void Game::Run() {
    std::cout << "Game running" << std::endl;
    while (running) {
        Update();
        if (shouldRender) {
            Render();
        }
    }
}

void Game::Update() {
    GameClock->Update();
    elapsedRender += GameClock->GetDeltaTime();
    if (elapsedRender >= renderTime) {
        shouldRender = true;
        elapsedRender -= renderTime;
    }
    
    Input->Update(GameClock->GetDeltaTime(), &running);
    
    if (Input->IsKeyDown(KeyEscape)) {
        running = false;
    }
    Grid->Update();
    
    //std::cout << "Game update at " << GameClock->GetLastTick() << " ticks." << std::endl;
}

void Game::Render() {
    shouldRender = false;
    Grid->Render();
    std::cout << "Game render." << std::endl;
}