#include "hexgrid.h"

HexGrid::HexGrid(Game* game) {
    gameRef = game;
    width = 6;
    length = 6;
    mesh = new Mesh(width, length);
    cells = new HexCell[width * length];
}

HexGrid::HexGrid(Game* game, GLuint gridWidth, GLuint gridLength) {
    gameRef = game;
    width = gridWidth;
    length = gridLength;
    mesh = new Mesh(width, length);
    cells = new HexCell[width * length];
}

HexGrid::~HexGrid() {
    delete mesh;
    delete [] cells;
}

void HexGrid::Initialize() {
    mesh->SetWidth(width);
    mesh->SetLength(length);
    mesh->Initialize();
    
    for (int z = 0, i = 0; z < length; z++) {
        for (int x = 0; x < width; x++) {
            CreateCell(x, z, i++);
        }
    }
    Triangulate();
}

void HexGrid::Update() {
    if (dirty) {
        Triangulate();
    }
}

void Triangulate() {
    mesh->Triangulate(cells);
}

void HexGrid::Render() {
    mesh->Render();
}

void HexGrid::CreateCell(GLint x, GLint z, GLint i) {
    glm::vec3 position;
    position.x = x * 10.0f;
    position.y = 0.0f;
    position.z = z * 10.0f;
    
    cells[i].Position = position;
}