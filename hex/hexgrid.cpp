#include "hexgrid.h"

HexGrid::HexGrid() {
    width = 6;
    length = 6;
    mesh = new Mesh(width, length);
    cells = new HexCell[width * length];
}

HexGrid::HexGrid(GLuint gridWidth, GLuint gridLength) {
    width = gridWidth;
    length = gridLength;
    mesh = new Mesh(width, length);
    cells = new HexCell[width * length];
}

HexGrid::~HexGrid() {
    delete mesh;
    delete[] cells;
}

void HexGrid::Initialize() {
    mesh->SetWidth(width);
    mesh->SetLength(length);
    mesh->Initialize();

    for (unsigned int z = 0, i = 0; z < length; z++) {
        for (unsigned int x = 0; x < width; x++) {
            CreateCell(x, z, i++);
        }
    }
    dirty = true;
    Triangulate();
}

void HexGrid::Update() {
    if (dirty) {
        Triangulate();
    }
}

void HexGrid::Triangulate() {
    mesh->Triangulate(cells);
    dirty = false;
}

void HexGrid::Render() {
    glm::mat4 MVP = Game::GameCamera->GetProjectionMatrix() *
                    Game::GameCamera->GetViewMatrix() *
                    glm::mat4(1.0f);
    mesh->Render(MVP);
}

void HexGrid::CreateCell(GLuint x, GLuint z, GLuint i) {
    glm::vec3 position;
    position.x = x * 10.0f;
    position.y = 0.0f;
    position.z = z * 10.0f;

    cells[i].Position = position;
}