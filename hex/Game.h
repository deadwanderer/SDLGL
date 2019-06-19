#ifndef _HEXGAME_H
#define _HEXGAME_H

#include "../include/Camera.h"
#include "Clock.h"
#include "InputManager.h"
#include "hexgrid.h"

#include "../include/SDL.h"
#include "../include/glad.h"

class Game {
    public:
    Game(SDL_Window *winPtr, unsigned int screenWidth, unsigned int screenHeight);
    ~Game();
    
    void Initialize();
    void Run();
    void Update();
    void Render();
    
    Camera *GameCamera;
    InputManager *Input;
    HexGrid *Grid;
    Clock *GameClock;
    
    unsigned int ScreenWidth;
    unsigned int ScreenHeight;
    
    private:
    GLboolean running;
    GLboolean shouldRender;
    SDL_Window *window;
    
    const float renderTime = 1.0f / 60.0f;
    float elapsedRender = 0.0f;
};

#endif