#include "../include/glm/glm.hpp"
#include "hex_metrics.h"

struct HexCell {
    int elevation;
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Color;
    HexCoordinates Coordinates;
    HexCell *neighbors[6];
    
    HexCell() {
        
    }
    
    HexCell* GetNeighbor(HexDirection direction) {
        return neighbors[direction];
    }
    
    void SetNeighbor(HexDirection direction, HexCell *cell) {
        neighbors[direction] = cell;
        cell->neighbors[GetHexDirOpposite(direction)] = *this;
    }
    
    HexEdgeType GetEdgeTypeFromDirection(HexDirection direction) {
        return HexMetrics.GetEdgeType(elevation, neighbors[direction]->elevation);
    }
    
    HexEdgeType GetEdgeTypeFromCell(HexCell *otherCell) {
        return HexMetrics.GetEdgeType(elevation, otherCell->elevation);
    }
    
    int GetElevation() {
        return elevation;
    }
    
    void SetElevation(int value) {
        elevation = value;
        glm::vec3 pos = Position;
        pos.y = value * HexMetrics.elevationStep;
        //pos.y += (HexMetrics.SampleNoise(pos).y * 2.0f - 1.0f) * HexMetrics.elevationPerturbStrength;
        Position = pos;
    }
};