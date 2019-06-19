#ifndef _HEXGRID_H
#define _HEXGRID_H

#include "hexcell.h"
#include "mesh.h"

class HexGrid {
    public:
    HexGrid(Game* game);
    HexGrid(Game* game, GLuint gridWidth, GLuint gridLength);
    ~HexGrid();
    
    void Initialize();
    void Update();
    void Triangulate();
    void Render();
    
    private:
    void CreateCell(GLint x, GLint z, GLint i);
    
    HexCell *cells;
    GLuint width, length;
    Mesh* mesh;
    Game* gameRef;
    GLboolean dirty;
};

#endif