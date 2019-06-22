#ifndef _MESH_H
#define _MESH_H

#include "../include/glad.h"
#include "../include/glm/glm.hpp"

#include <vector>

#include "../include/Shader.h"
#include "hex_metrics.h"

struct MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;

    void Clear() {
        vertices.clear();
        normals.clear();
        colors.clear();
    }
};

struct TexturedMeshData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
};

class Mesh {
  public:
    Mesh();
    Mesh(GLuint cellsWide, GLuint cellsLong);
    ~Mesh();

    void Initialize();
    void Triangulate(HexCell *cells, unsigned int cellCount);
    void Triangulate(HexCell *cell);
    void Triangulate(HexDirection direction, HexCell *cell);
    void Update();
    void Render(glm::mat4 MVP);

    void SetWidth(GLuint newWidth);
    void SetLength(GLuint newLength);

    MeshData meshData;
    std::vector<GLuint> indices;
    MeshData boxData;

  private:
    void AddTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void AddTriangleColors(glm::vec3 color);
    void AddTriangleColors(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3);
    void AddQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
    void AddQuadColors(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);
    void RecalculateNormals();

    GLuint CalculateVertexBufferMaxSize();
    GLuint CalculateIndexBufferMaxSize();

    GLuint width, length;

    Shader meshShader;
    GLuint meshVAO, meshVerticesVBO, meshNormalsVBO, meshColorsVBO, meshEBO;
    GLuint vertexCount, indexCount, normalCount, colorCount;
    GLuint lastVertexCount, lastIndexCount, lastNormalCount, lastColorCount;
    GLuint maxVertexBufferSize, maxIndexBufferSize;

    GLboolean dirty;
    GLboolean verticesChanged;
    GLboolean indicesChanged;
};

#endif