#include "Game.h"

#include <iostream>

Game::Game(SDL_Window *winPtr, unsigned int screenWidth, unsigned int screenHeight)
{
    window = winPtr;
    this->ScreenWidth = screenWidth;
    this->ScreenHeight = screenHeight;
    //std::cout << "Created game." << std::endl;
}

Game::~Game()
{
    delete GameCamera;
    delete Input;
    delete GameClock;
}

void Game::Initialize()
{
    GameCamera = new Camera(glm::vec3(2.0f, 2.0f, 5.0f));
    GameCamera->AspectRatio = (float)ScreenWidth / (float)ScreenHeight;
    Input = new InputManager((unsigned int)ScreenWidth / 2, (unsigned int)ScreenHeight / 2);
    GameClock = new Clock();
    GameClock->Initialize();
    Grid = new HexGrid(GameCamera);
    Grid->Initialize();

    running = true;
    shouldRender = false;
    //std::cout << "Initialization complete." << std::endl;
}

void Game::Run()
{
    //std::cout << "Game running" << std::endl;
    while (running)
    {
        Update();
        if (shouldRender)
        {
            Render();
        }
    }
}

void Game::Update()
{
    GameClock->Update();
    elapsedRender += GameClock->GetDeltaTime();
    if (elapsedRender >= renderTime)
    {
        //std::cout << "Elapsed time: " << elapsedRender << std::endl;
        shouldRender = true;
        elapsedRender -= renderTime;
    }

    Input->Update(GameClock->GetDeltaTime(), &running);

    if (Input->IsKeyDown(KeyEscape))
    {
        running = false;
    }
    if (Input->IsKeyDown(KeyW))
    {
        GameCamera->ProcessKeyboard(FORWARD, GameClock->GetDeltaTime());
    }
    if (Input->IsKeyDown(KeyS))
    {
        GameCamera->ProcessKeyboard(BACKWARD, GameClock->GetDeltaTime());
    }
    if (Input->IsKeyDown(KeyA))
    {
        GameCamera->ProcessKeyboard(LEFT, GameClock->GetDeltaTime());
    }
    if (Input->IsKeyDown(KeyD))
    {
        GameCamera->ProcessKeyboard(RIGHT, GameClock->GetDeltaTime());
    }
    if (Input->IsKeyDown(KeyQ) || Input->IsKeyDown(KeySpace))
    {
        GameCamera->ProcessKeyboard(UP, GameClock->GetDeltaTime());
    }
    if (Input->IsKeyDown(KeyE) || Input->IsKeyDown(KeyLShift))
    {
        GameCamera->ProcessKeyboard(DOWN, GameClock->GetDeltaTime());
    }
    if (Input->WasKeyJustPressed(KeyF))
    {
        Input->ToggleCaptureMouse();
    }
    MousePos mouseMovement = Input->GetMouseMovement();
    GameCamera->ProcessMouseMovement((float)mouseMovement.X, (float)-mouseMovement.Y);

    Grid->Update();

    //std::cout << "Game update at " << GameClock->GetLastTick() << " ticks." << std::endl;
}

void Game::Render()
{
    shouldRender = false;
    glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_DEPTH_TEST);

    Grid->Render();

    SDL_GL_SwapWindow(window);
    //std::cout << "Game render." << std::endl;
}