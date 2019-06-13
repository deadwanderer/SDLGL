#include "../include/Camera.h"
#include "hexgrid.h"
#include "InputManager.h"

class Game {
    public:
    Game();
    ~Game();
    
    void Initialize();
    void Update();
    void Render();
    
    Camera Camera;
    
    private:
    
};
