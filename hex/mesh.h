#ifndef _MESH_H
#define _MESH_H

#include "../include/glad.h"
#include "../include/glm/glm.hpp"

#include <vector>

#include "hex_metrics.h"
#include "../include/Shader.h"

struct MeshData {
    std::vector<glm::vec3> vertices;
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
    public:
    Mesh();
    Mesh(GLuint cellsWide, GLuint cellsLong);
    ~Mesh();
    
    void Initialize();
    void Triangulate();
    void Update();
    void Render();
    
    void SetWidth(GLuint newWidth);
    void SetLength(GLuint newLength);
    
    MeshData meshData;
    std::vector<GLuint> indices;
    
    private:
    void AddTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void AddTriangleColor(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3);
    void AddQuad(glm::vec3 v1, glm::vec2 v2, glm::vec3 v3, glm::vec3 v4);
    void AddQuadColor(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
    void RecalculateNormals();
    
    GLuint CalculateVertexBufferMaxSize();
    GLuint CalculateIndexBufferMaxSize();
    
    GLuint width, length;
    
    Shader meshShader;
    GLuint meshVAO, meshVBO, meshEBO;
    GLuint vertexCount, indexCount, normalCount, colorCount;
    GLuint maxVertexBufferSize, maxIndexBufferSize;
    
    GLboolean dirty;
};

#endif