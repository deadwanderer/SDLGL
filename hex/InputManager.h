#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include "../include/glad.h"

class Game;

enum GameKeys {
    KeyW = 0,
    KeyS = 1,
    KeyD = 2,
    KeyA = 3,
    KeyQ = 4,
    KeyE = 5,
    KeyZ = 6,
    KeyX = 7,
    KeyC = 8,
    KeyV = 9,
    KeyF = 10,
    KeyR = 11,
    KeyEscape = 12,
    KeyEnter = 13,
    KeySpace = 14,
    KeyLShift = 15,
    
    KeyCount = 16
};

class InputManager {
    public:
    InputManager(Game* game);
    ~InputManager();
    
    void Update(float dt, GLboolean* running);
    void SetHoldActivateTime(float newHoldTime);
    GLboolean IsKeyDown(GLuint key);
    GLboolean IsKeyUp(GLuint key);
    GLboolean WasKeyJustPressed(GLuint key);
    GLboolean WasKeyJustReleased(GLuint key);
    GLboolean IsHeldKey(GLuint key);
    float TimeHeldFor(GLuint key);
    
    private:
    GLboolean lastState[KeyCount];
    GLboolean currentState[KeyCount];
    float timeDown[KeyCount];
    float holdTime;
    Game* game;
};

#endif