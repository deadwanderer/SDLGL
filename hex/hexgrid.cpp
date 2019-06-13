#include "hexgrid.h"

HexGrid::HexGrid() {
    width = 6;
    length = 6;
}

HexGrid::HexGrid(GLuint gridWidth, GLuint gridLength) {
    width = gridWidth;
    length = gridLength;
}

HexGrid::~HexGrid() {
    
}

HexGrid::Initialize() {
    mesh.SetWidth(width);
    mesh.SetLength(length);
    mesh.Initialize();
}