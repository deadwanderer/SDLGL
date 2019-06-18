#include "mesh.h"

Mesh::Mesh() : meshShader("meshShader.vs", "meshShader.fs") {
    width = length = 5;
    
    vertexCount = indexCount = normalCount = colorCount = 0;
    dirty = true;
}

Mesh::Mesh(GLuint cellsWide, GLuint cellsLong) : meshShader("meshShader.vs", "meshShader.fs") {
    width = cellsWide;
    length = cellsLong;
    
    vertexCount = indexCount = normalCount = colorCount = 0;
    dirty = true;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &meshEBO);
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);
}

void Mesh::Triangulate(HexCell* cells, unsigned int cellCount) {
    lastVertexCount = meshData.vertices.size();
    lastNormalCount = meshData.normals.size();
    lastColorCount = meshData.colors.size();
    lastIndexCount = indices.size();
    
    meshData.Clear();
    indices.clear();
    
    for (unsigned int i = 0; i < cellCount; i++) {
        Triangulate(cells[i]);
    }
    
    RecalculateNormals();
}

void Mesh::Triangulate(HexCell *cell) {
    
}

void Mesh::Update() {
    if (dirty) {
        Triangulate();
    }
}

void Mesh::Render() {
    
}

void Mesh::Initialize() {
    // First, calculate a max buffer size.
    maxVertexBufferSize = CalculateVertexBufferMaxSize();
    maxIndexBufferSize = CalculateIndexBufferMaxSize();
    
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);
    
    glBindVertexArray(meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, maxVertexBufferSize * sizeof(GLfloat), 0, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexBufferSize * sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
    
    // TODO(anthony): Complete mesh initialization
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetWidth(GLuint newWidth) {
    width = newWidth;
}

void Mesh::SetLength(GLuint newLength) {
    length = newLength;
}

void Mesh::AddTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    unsigned int vertexCount = meshData.vertices.size();
    meshData.vertices.push_back(v1);
    meshData.vertices.push_back(v2);
    meshData.vertices.push_back(v3);
    indices.push_back(vertexCount++);
    indices.push_back(vertexCount++);
    indices.push_back(vertexCount++);
}

void Mesh::AddTriangleColors(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3) {
    meshData.colors.push_back(c1);
    meshData.colors.push_back(c2);
    meshData.colors.push_back(c3);
}

void Mesh::AddQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
    unsigned int vertexCount = meshData.vertices.size();
    meshData.vertices.push_back(v1);
    meshData.vertices.push_back(v2);
    meshData.vertices.push_back(v3);
    meshData.vertices.push_back(v4);
    
    // TODO(anthony): Add indices!
}

void Mesh::AddQuadColors(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4) {
    meshData.colors.push_back(c1);
    meshData.colors.push_back(c2);
    meshData.colors.push_back(c3);
    meshData.colors.push_back(c4);
}

void Mesh::RecalculateNormals() {
    
}

GLuint Mesh::CalculateVertexBufferMaxSize() {
    GLuint floatsPerVertex = 9;  // 3 for position, three for normal, three for color
    GLuint verticesPerCell = 18; // 3 vertices per tri * 6 tris per hex
    GLuint bufferAmount = 2;     // multiply the result by this amount for fudging
    
    return width * length * verticesPerCell * floatsPerVertex * bufferAmount;
}

GLuint Mesh::CalculateIndexBufferMaxSize() {
    GLuint indicesPerCell = 18;
    GLuint bufferAmount = 2;
    
    return width * length * indicesPerCell * bufferAmount;
}