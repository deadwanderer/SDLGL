#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include "../include/glad.h"

enum GameKeys
{
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

enum MouseButtons
{
    LeftButton = 0,
    MiddleButton = 1,
    RightButton = 2,

    ButtonCount = 3
};

struct MousePos
{
    int X;
    int Y;
};

class InputManager
{
public:
    InputManager(int centerX, int centerY);
    ~InputManager();

    void Update(float dt, GLboolean *running);
    void SetHoldActivateTime(float newHoldTime);
    void SetKeyHoldActivateTime(float newKeyHoldTime);
    void SetMouseHoldActivateTime(float newMouseHoldTime);
    void ToggleCaptureMouse();
    GLboolean IsMouseCaptured();
    GLboolean IsKeyDown(GameKeys key);
    GLboolean IsKeyUp(GameKeys key);
    GLboolean WasKeyJustPressed(GameKeys key);
    GLboolean WasKeyJustReleased(GameKeys key);
    GLboolean IsHeldKey(GameKeys key);
    float TimeKeyHeldFor(GameKeys key);
    GLboolean IsButtonDown(MouseButtons button);
    GLboolean IsButtonUp(MouseButtons button);
    GLboolean WasButtonJustPressed(MouseButtons button);
    GLboolean WasButtonJustReleased(MouseButtons button);
    GLboolean IsHeldButton(MouseButtons button);
    float TimeButtonHeldFor(MouseButtons button);
    GLboolean DidMouseMove();
    MousePos GetMouseMovement();

private:
    void
    ChangeSDLMouseCapture();

    GLboolean lastKeyState[KeyCount];
    GLboolean currentKeyState[KeyCount];
    GLboolean lastMouseState[ButtonCount];
    GLboolean currentMouseState[ButtonCount];
    MousePos lastMousePosition;
    MousePos currentMousePosition;
    MousePos mouseMovement;
    MousePos screenCenter;
    float keyTimeDown[KeyCount];
    float mouseTimeDown[ButtonCount];
    float keyHoldTime;
    float mouseHoldTime;
    GLboolean captureMouse;
};

#endif