#ifndef _HEXGAME_H
#define _HEXGAME_H

#include "../include/Camera.h"
#include "hexgrid.h"
#include "InputManager.h"
#include "Clock.h"

#include "../include/glad.h"

class Game {
    public:
    Game();
    ~Game();
    
    void Initialize();
    void Run();
    void Update();
    void Render();
    
    Camera* GameCamera;
    InputManager* Input;
    HexGrid* Grid;
    Clock* GameClock;
    
    private:
    GLboolean running;
    GLboolean shouldRender;
    
    const float renderTime = 1.0f / 60.0f;
    float elapsedRender = 0.0f;
};

#endif