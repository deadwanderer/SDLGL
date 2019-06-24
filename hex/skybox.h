#include "Shader.h"
#include "../include/glm/glm.hpp"
#include "../include/glad.h"
#include <string>
#include <vector>

class Skybox {
    public:
    Skybox(const Shader& skyboxShader);
    ~Skybox();
    
    void Initialize();
    void Render();
    
    private:
    Shader shader;
    std::vector<std::string> faces;
};