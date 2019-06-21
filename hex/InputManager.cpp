#include "InputManager.h"

#include "../include/SDL.h"

#include <iostream>

InputManager::InputManager(int centerX, int centerY) {
    screenCenter.X = centerX;
    screenCenter.Y = centerY;
    keyHoldTime = 0.3f;
    mouseHoldTime = 0.3f;
    captureMouse = GL_FALSE;
    for (GLuint i = 0; i < KeyCount; i++) {
        lastKeyState[i] = GL_FALSE;
        currentKeyState[i] = GL_FALSE;
        keyTimeDown[i] = 0.0f;
    }
    for (GLuint i = 0; i < ButtonCount; i++) {
        lastMouseState[i] = GL_FALSE;
        currentMouseState[i] = GL_FALSE;
        mouseTimeDown[i] = 0.0f;
    }

    SDL_GetMouseState(&currentMousePosition.X, &currentMousePosition.Y);
    lastMousePosition.X = currentMousePosition.X;
    lastMousePosition.Y = currentMousePosition.Y;
}

InputManager::~InputManager() {
}

void InputManager::Update(float dt, GLboolean *running) {
    mouseMovement.X = 0;
    mouseMovement.Y = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        } else if (event.type == SDL_MOUSEMOTION) {
            mouseMovement.X = event.motion.xrel;
            mouseMovement.Y = event.motion.yrel;
        }
    }

    const Uint8 *keyState = SDL_GetKeyboardState(NULL);

    for (GLuint i = 0; i < KeyCount; i++) {
        lastKeyState[i] = currentKeyState[i];
        currentKeyState[i] = GL_FALSE;
    }

    if (keyState[SDL_SCANCODE_W]) {
        currentKeyState[KeyW] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_S]) {
        currentKeyState[KeyS] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_D]) {
        currentKeyState[KeyD] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_A]) {
        currentKeyState[KeyA] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_Q]) {
        currentKeyState[KeyQ] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_E]) {
        currentKeyState[KeyE] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_Z]) {
        currentKeyState[KeyZ] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_X]) {
        currentKeyState[KeyX] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_C]) {
        currentKeyState[KeyC] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_V]) {
        currentKeyState[KeyV] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_F]) {
        currentKeyState[KeyF] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_R]) {
        currentKeyState[KeyR] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_ESCAPE]) {
        currentKeyState[KeyEscape] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_RETURN]) {
        currentKeyState[KeyEnter] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_SPACE]) {
        currentKeyState[KeySpace] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_LSHIFT]) {
        currentKeyState[KeyLShift] = GL_TRUE;
    }

    for (GLuint i = 0; i < KeyCount; i++) {
        if (lastKeyState[i] == GL_TRUE && currentKeyState[i] == GL_TRUE) {
            keyTimeDown[i] += dt;
        } else {
            keyTimeDown[i] = 0.0f;
        }
    }

    lastMousePosition.X = currentMousePosition.X;
    lastMousePosition.Y = currentMousePosition.Y;

    Uint32 mouseButtonState = SDL_GetMouseState(&currentMousePosition.X, &currentMousePosition.Y);
    if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        currentMouseState[LeftButton] = GL_TRUE;
    }
    if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        currentMouseState[MiddleButton] = GL_TRUE;
    }
    if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        currentMouseState[RightButton] = GL_TRUE;
    }
}

void InputManager::SetHoldActivateTime(float newHoldTime) {
    if (newHoldTime <= 0.0f) {
        std::cout << "Error: Hold Time must be greater than 0!" << std::endl;
        return;
    }
    SetKeyHoldActivateTime(newHoldTime);
    SetMouseHoldActivateTime(newHoldTime);
}

void InputManager::SetKeyHoldActivateTime(float newKeyHoldTime) {
    if (newKeyHoldTime <= 0.0f) {
        std::cout << "Error: Key Hold Time must be greater than 0.0f!" << std::endl;
        return;
    }
    keyHoldTime = newKeyHoldTime;
}

void InputManager::SetMouseHoldActivateTime(float newMouseHoldTime) {
    if (newMouseHoldTime <= 0.0f) {
        std::cout << "Error: Mouse Hold Time must be greater than 0.0f!" << std::endl;
        return;
    }
    mouseHoldTime = newMouseHoldTime;
}

GLboolean InputManager::IsKeyDown(GameKeys key) {
    return currentKeyState[key];
}

GLboolean InputManager::IsKeyUp(GameKeys key) {
    return !currentKeyState[key];
}

GLboolean InputManager::WasKeyJustPressed(GameKeys key) {
    return (lastKeyState[key] == GL_FALSE && currentKeyState[key] == GL_TRUE);
}

GLboolean InputManager::WasKeyJustReleased(GameKeys key) {
    return (lastKeyState[key] == GL_TRUE && currentKeyState[key] == GL_FALSE);
}

GLboolean InputManager::IsHeldKey(GameKeys key) {
    return keyTimeDown[key] >= keyHoldTime;
}

float InputManager::TimeKeyHeldFor(GameKeys key) {
    return keyTimeDown[key];
}

GLboolean InputManager::IsButtonDown(MouseButtons button) {
    return currentMouseState[button];
}

GLboolean InputManager::IsButtonUp(MouseButtons button) {
    return !currentMouseState[button];
}

GLboolean InputManager::WasButtonJustPressed(MouseButtons button) {
    return (lastMouseState[button] == GL_FALSE && currentMouseState[button] == GL_TRUE);
}

GLboolean InputManager::WasButtonJustReleased(MouseButtons button) {
    return (lastMouseState[button] == GL_TRUE && currentMouseState[button] == GL_FALSE);
}

GLboolean InputManager::IsHeldButton(MouseButtons button) {
    return mouseTimeDown[button] >= mouseHoldTime;
}

float InputManager::TimeButtonHeldFor(MouseButtons button) {
    return mouseTimeDown[button];
}

GLboolean InputManager::DidMouseMove() {
    if (captureMouse) {
        return (mouseMovement.X != 0 || mouseMovement.Y != 0);
    } else {
        return (currentMousePosition.X != lastMousePosition.X || currentMousePosition.Y != lastMousePosition.Y);
    }
}

MousePos InputManager::GetMouseMovement() {
    if (captureMouse) {
        return mouseMovement;
    } else {
        MousePos result;
        result.X = currentMousePosition.X - lastMousePosition.X;
        result.Y = currentMousePosition.Y - lastMousePosition.Y;
        return result;
    }
}

void InputManager::ToggleCaptureMouse() {
    captureMouse = !captureMouse;
    ChangeSDLMouseCapture();
}

GLboolean InputManager::IsMouseCaptured() {
    return captureMouse;
}

void InputManager::ChangeSDLMouseCapture() {
    SDL_SetRelativeMouseMode(captureMouse == GL_TRUE ? SDL_TRUE : SDL_FALSE);
}