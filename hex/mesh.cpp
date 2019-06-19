#include "mesh.h"

Mesh::Mesh(Game* gameRef) : meshShader("meshShader.vs", "meshShader.fs") {
    this->gameRef = gameRef;
    width = length = 5;
    
    vertexCount = indexCount = normalCount = colorCount = 0;
    dirty = true;
}

Mesh::Mesh(Game* gameRef, GLuint cellsWide, GLuint cellsLong) : meshShader("meshShader.vs", "meshShader.fs") {
    this->gameRef = gameRef;
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
    lastVertexCount = (GLuint)meshData.vertices.size();
    lastNormalCount = (GLuint)meshData.normals.size();
    lastColorCount = (GLuint)meshData.colors.size();
    lastIndexCount = (GLuint)indices.size();
    
    meshData.Clear();
    indices.clear();
    
    for (unsigned int i = 0; i < cellCount; i++) {
        Triangulate(&cells[i]);
    }
    
    RecalculateNormals();
    verticesChanged = true;
    indicesChanged = true;
}

void Mesh::Triangulate(HexCell *cell) {
    glm::vec3 center = cell->Position;
    for (unsigned int i = 0; i < 6; i++) {
        AddTriangle(
            center,
            center + HexMetrics.corners[i],
            center + HexMetrics.corners[i+1]
            );
        AddTriangleColors(glm::vec3(0.8f, 0.2f, 0.3f));
    }
    /*
for (HexDirection d = HexDirection::NE; d <= HexDirection::NW; d++) {
        Triangulate(d, cell);
    }
*/
}

void Mesh::Triangulate(HexDirection direction, HexCell* cell) {
    glm::vec3 center = cell->Position;
}

void Mesh::Update() {
    
}

void Mesh::Render() {
    glBindVertexArray(meshVAO);
    if (verticesChanged) {
        size_t vertSize = meshData.vertices.size() * 3 * sizeof(float);
        size_t normSize = meshData.normals.size() * 3 * sizeof(float);
        size_t colorSize = meshData.colors.size() * 3 * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, &meshData.vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, &meshData.normals[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertSize + normSize, colorSize, &meshData.colors[0]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertSize);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertSize + normSize));
    }
    if (indicesChanged) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);
    }
    
    meshShader.use();
    glm::mat4 MVP = gameRef->GameCamera->GetProjectionMatrix() * 
        gameRef->GameCamera->GetViewMatrix() * glm::mat4(1.0);
    meshShader.setMat4("MVP", MVP);
    
    glDrawElements(GL_TRIANGLES, indices.size() / 3, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
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
    meshData.normals.push_back(glm::vec3(0.0f));
    meshData.normals.push_back(glm::vec3(0.0f));
    meshData.normals.push_back(glm::vec3(0.0f));
    indices.push_back(vertexCount++);
    indices.push_back(vertexCount++);
    indices.push_back(vertexCount++);
}

void Mesh::AddTriangleColors(glm::vec3 color) {
    meshData.colors.push_back(color);
    meshData.colors.push_back(color);
    meshData.colors.push_back(color);
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
    meshData.normals.push_back(glm::vec3(0.0f));
    meshData.normals.push_back(glm::vec3(0.0f));
    meshData.normals.push_back(glm::vec3(0.0f));
    meshData.normals.push_back(glm::vec3(0.0f));
    
    // TODO(anthony): Add indices!
}

void Mesh::AddQuadColors(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4) {
    meshData.colors.push_back(c1);
    meshData.colors.push_back(c2);
    meshData.colors.push_back(c3);
    meshData.colors.push_back(c4);
}

void Mesh::RecalculateNormals() {
    for (unsigned int i = 0; i < indices.size() / 3; i++) {
        int index1 = indices[i*3];
        int index2 = indices[i*3+1];
        int index3 = indices[i*3+2];
        
        glm::vec3 side1 = meshData.vertices[index1] - meshData.vertices[index3];
        glm::vec3 side2 = meshData.vertices[index1] - meshData.vertices[index2];
        glm::vec3 normal = glm::cross(side1, side2);
        
        meshData.normals[index1] += normal;
        meshData.normals[index2] += normal;
        meshData.normals[index3] += normal;
    }
    
    for (unsigned int i = 0; i < meshData.normals.size(); i++) {
        meshData.normals[i] = glm::normalize(meshData.normals[i]);
    }
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