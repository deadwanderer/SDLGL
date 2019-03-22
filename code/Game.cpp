#include "Game.h"
#include "Resource_Manager.h"
#include "SpriteRenderer.h"
#include "../include/SDL.h"

// Game-related State data
SpriteRenderer *Renderer;

Game::Game(GLuint width, GLuint height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
    
}

Game::~Game() {
    delete Renderer;
}

void Game::Init() {
    quit = false;
    
    // Load shaders
    ResourceManager::LoadShader("sprite", "C:/SDL/code/sprite.vs", "C:/SDL/code/sprite.fs", nullptr);
    
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    
    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    
    // Load textures
    ResourceManager::LoadTexture("face", "../res/awesomeface.png", GL_TRUE);
}

void Game::Update(GLfloat dt) {
    
}

void Game::ProcessInput(GLfloat dt) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN &&
             event.key.keysym.sym == SDLK_ESCAPE)) {
            quit = true;
        }
        
        /*
        if (event.type == SDL_MOUSEMOTION && captureMouse) {
            //camera.ProcessMouseMovement((float)event.motion.xrel, (float)-event.motion.yrel);
        }
        if (event.type == SDL_MOUSEWHEEL && captureMouse) {
            //camera.ProcessMouseScroll((float)event.wheel.y);
        }
        */
    }
    
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    //lastPressed += deltaTime;
    
    if (keystate[SDL_SCANCODE_W]) {
        
    }
    if (keystate[SDL_SCANCODE_S]) {
        
    }
    if (keystate[SDL_SCANCODE_A]) {
        
    }
    if (keystate[SDL_SCANCODE_D]) {
        
    }
    if (keystate[SDL_SCANCODE_Q] || keystate[SDL_SCANCODE_SPACE]) {
        
    }
    if (keystate[SDL_SCANCODE_E] || keystate[SDL_SCANCODE_LSHIFT]) {
        
    }
    /*
    if (keystate[SDL_SCANCODE_F] && lastPressed >= keyCooldown) {
    
    
    }
    */
}

void Game::Render() {
    Renderer->DrawSprite(ResourceManager::GetTexture("face"),
                         glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

bool Game::ShouldQuit() {
    return quit;
}