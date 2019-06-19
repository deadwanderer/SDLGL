#include "InputManager.h"

#include "../include/SDL.h"

#include <iostream>

InputManager::InputManager(Game* game) {
    this->game = game;
    holdTime = 0.3f;
    for (GLuint i = 0; i < KeyCount; i++) {
        lastState[i] = GL_FALSE;
        currentState[i] = GL_FALSE;
        timeDown[i] = 0.0f;
    }
}

InputManager::~InputManager() {
    
}

void InputManager::Update(float dt, GLboolean *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
    }
    
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    
    for (GLuint i = 0; i < KeyCount; i++) {
        lastState[i] = currentState[i];
        currentState[i] = GL_FALSE;
    }
    
    if (keyState[SDL_SCANCODE_W]) {
        currentState[KeyW] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_S]) {
        currentState[KeyS] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_D]) {
        currentState[KeyD] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_A]) {
        currentState[KeyA] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_Q]) {
        currentState[KeyQ] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_E]) {
        currentState[KeyE] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_Z]) {
        currentState[KeyZ] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_X]) {
        currentState[KeyX] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_C]) {
        currentState[KeyC] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_V]) {
        currentState[KeyV] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_F]) {
        currentState[KeyF] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_R]) {
        currentState[KeyR] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_ESCAPE]) {
        currentState[KeyEscape] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_RETURN]) {
        currentState[KeyEnter] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_SPACE]) {
        currentState[KeySpace] = GL_TRUE;
    }
    if (keyState[SDL_SCANCODE_LSHIFT]) {
        currentState[KeyLShift] = GL_TRUE;
    }
    
    for (GLuint i = 0; i < KeyCount; i++) {
        if (lastState[i] == GL_TRUE && currentState[i] == GL_TRUE) {
            timeDown[i] += dt;
        }
        else {
            timeDown[i] = 0.0f;
        }
    }
}

void InputManager::SetHoldActivateTime(float newHoldTime) {
    if (newHoldTime <= 0.0f) {
        std::cout << "Error: Key Hold Time must be greater than 0!" << std::endl;
        return;
    }
    
    holdTime = newHoldTime;
}

GLboolean InputManager::IsKeyDown(GLuint key) {
    return currentState[key];
}

GLboolean InputManager::IsKeyUp(GLuint key) {
    return !currentState[key];
}

GLboolean InputManager::WasKeyJustPressed(GLuint key) {
    return (lastState[key] == GL_FALSE && currentState[key] == GL_TRUE);
}

GLboolean InputManager::WasKeyJustReleased(GLuint key) {
    return (lastState[key] == GL_TRUE && currentState[key] == GL_FALSE);
}

GLboolean InputManager::IsHeldKey(GLuint key) {
    return timeDown[key] >= holdTime;
}

float InputManager::TimeHeldFor(GLuint key) {
    return timeDown[key];
}