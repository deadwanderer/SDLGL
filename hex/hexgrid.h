#ifndef _HEXGRID_H
#define _HEXGRID_H

#include "../include/Camera.h"
#include "mesh.h"

class HexGrid {
  public:
    HexGrid(Camera *gameCamera) {
        cam = gameCamera;
        width = 6;
        length = 6;
        cellCount = width * length;
        mesh = new Mesh(width, length);
        cells = new HexCell[cellCount];
    }

    HexGrid(Camera *gameCamera, GLuint gridWidth, GLuint gridLength) {
        cam = gameCamera;
        width = gridWidth;
        length = gridLength;
        cellCount = width * length;
        mesh = new Mesh(width, length);
        cells = new HexCell[cellCount];
    }
    ~HexGrid() {
        delete mesh;
        delete[] cells;
    }

    void Initialize() {
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

    void Update() {
        if (dirty) {
            Triangulate();
        }
    }

    void Triangulate() {
        mesh->Triangulate(cells, cellCount);
        dirty = false;
    }

    void Render() {
        glm::mat4 MVP = cam->GetProjectionMatrix() *
                        cam->GetViewMatrix() *
                        glm::mat4(1.0f);
        mesh->Render(MVP);
    }

  private:
    void CreateCell(GLuint x, GLuint z, GLuint i) {
        glm::vec3 position;
        position.x = x * 10.0f;
        position.y = 0.0f;
        position.z = z * 10.0f;

        cells[i].Position = position;
    }

    HexCell *cells;
    GLuint width, length, cellCount;
    Mesh *mesh;
    GLboolean dirty;
    Camera *cam;
};

#endif