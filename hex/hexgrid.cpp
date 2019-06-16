#include "hexgrid.h"

HexGrid::HexGrid(Game* game) {
    gameRef = game;
    width = 6;
    length = 6;
    mesh = new Mesh(width, length);
}

HexGrid::HexGrid(Game* game, GLuint gridWidth, GLuint gridLength) {
    gameRef = game;
    width = gridWidth;
    length = gridLength;
    mesh = new Mesh(width, length);
}

HexGrid::~HexGrid() {
    
}

void HexGrid::Initialize() {
    mesh->SetWidth(width);
    mesh->SetLength(length);
    mesh->Initialize();
}

void HexGrid::Update() {
    
}

void HexGrid::Render() {
    
}

void HexGrid::CreateCell(GLint x, GLint z, GLint i) {
    
}