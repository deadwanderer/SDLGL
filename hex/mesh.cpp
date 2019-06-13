#include "mesh.h"

Mesh::Mesh() : meshShader("../res/shaders/meshShader.vs", "../res/shaders/meshShader.fs") {
    width = length = 5;
    
    vertexCount = indexCount = normalCount = colorCount = 0;
    dirty = true;
}

Mesh::Mesh(GLuint cellsWide, GLuint cellsLong) : meshShader("../res/shaders/meshShader.vs", "../res/shaders/meshShader.fs") {
    width = cellsWide;
    length = cellsLong;
    
    vertexCount = indexCount = normalCount = colorCount = 0;
    dirty = true;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, meshEBO);
    glDeleteBuffers(1, meshVBO);
    glDeleteVertexArrays(1, meshVAO);
}

void Mesh::Triangulate() {
    
}

void Mesh::Update() {
    if(dirty) {
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



GLuint Mesh::CalculateVertexBufferMaxSize() {
    GLuint floatsPerVertex = 9; // 3 for position, three for normal, three for color
    GLuint verticesPerCell = 18; // 3 vertices per tri * 6 tris per hex
    GLuint bufferAmount = 2; // multiply the result by this amount for fudging
    
    return width * height * verticesPerCell * floatsPerVertex * bufferAmount;
}

GLuint Mesh::CalculateIndexBufferMaxSize() {
    GLuint indicesPerCell = 18;
    GLuint bufferAmount = 2;
    
    return width * height * indicesPerCell * bufferAmount;
}