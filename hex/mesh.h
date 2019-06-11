#ifndef _MESH_H
#define _MESH_H

#include "../include/glm/glm.hpp"
#include <vector>

struct MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
};

struct TexturedMeshData {
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
};

class Mesh {
    
};

#endif