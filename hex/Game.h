#include "../include/Camera.h"
#include "hexgrid.h"
#include "InputManager.h"

class Game {
    public:
    Game();
    ~Game();
    
    void Initialize();
    void Run();
    void Update();
    void Render();
    
    Camera* Camera;
    InputManager* Input;
    
    private:
    
};
